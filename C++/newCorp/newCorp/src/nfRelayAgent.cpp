/*
 * nfRelayAgent.cpp - NF Relay Agent
 * usage: nfRelayAgent <binding port> <dest IP> <dest Port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <NfDecodeThread.h>

#define BUFSIZE 65535

#include <iostream>
#include <sstream>

using namespace std;
using std::cout;
using std::endl;

long processNF9Packet(u_char *buf, int len, u_int32_t cliRouterIpAddr)
{
}

int main(int argc, char **argv)
{
  int sockfd; /* socket */
  u_int16_t port; /* port to listen on */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *clientaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int udpDataGramLen; /* message byte size */

  /* 
   * check command line arguments 
   */
  if (argc != 4) {
    error("usage: nfRelayAgent <binding port> <dest IP> <dest Port>");
  }
  port = atoi(argv[1]);

  //Retrieve Relay IP & Port
  u_int16_t fwdPort = htons(atoi(argv[3]));
  u_int32_t fwdIP = 0;
  if((fwdIP = inet_addr(argv[2])) == (unsigned long)INADDR_NONE)
  {
    /* When passing the host name of the server as a */
    /* parameter to this program, use the gethostbyname() */
    /* function to retrieve the address of the host server. */
    /***************************************************/
    /* get host address */
    hostp = gethostbyname(argv[2]);
    if(hostp == (struct hostent *)NULL)
    {
      printf("HOST NOT FOUND --> ");
      /* h_errno is usually defined */
      /* in netdb.h */
      printf("h_errno = %d\n",h_errno);
      close(sockfd);
      exit(-1);
    }
    memcpy(&fwdIP, hostp->h_addr, sizeof(u_int32_t));
  }

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error((char*)"ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
    error((char*)"ERROR setsockopt: SO_REUSEADDR");

  /*
   * getsockopt: Query UDP socket's SNDBUF and RCVBUF
   */
  int sndbuf = 0;
  int rcvbuf = 0;
  int optlen = sizeof optval;
  if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *)&sndbuf, (socklen_t*)&optlen) < 0)
    error((char*)"ERROR getsockopt: SO_SNDBUF");
  if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&rcvbuf, (socklen_t*)&optlen) < 0)
    error((char*)"ERROR getsockopt: SO_RCVBUF");
  printf("Querying socket SNDBUF = %d, RCVBUF = %d\n", sndbuf, rcvbuf);

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (::bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    error((char*)"ERROR on binding");

  //Start NF RelayThread
  NfRelayThread::singleton()->Start();
  NfRelayThread::singleton()->setRelayEnd(fwdIP, fwdPort);

  //Start NF MonitorThread
  NfRelayThread::singleton()->sharedMemoryKey(2222);
  NfMonitorThread::singleton()->setThreadToBeMonitored(NfRelayThread::singleton());
  NfMonitorThread::singleton()->Start();

  /* 
   * main loop: wait for a datagram
   */
  clientlen = sizeof(clientaddr);

  const char* packet = NULL;
  while (1) {
    /*
     * recvfrom: receive a UDP datagram from a NetFlow Exportor
     */
    bzero(buf, BUFSIZE);
    udpDataGramLen = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (udpDataGramLen < 0)
      error((char*)"ERROR in recvfrom");

    /*
     * inet_ntoa: determine where clients come from
     */
    clientaddrp = inet_ntoa(clientaddr.sin_addr);
    if (clientaddrp == NULL)
      error((char*)"ERROR on inet_ntoa\n");

    packet = buf;
    // enqueue netflow v9 record here
    // piggyback client port
    memcpy(((u_int8_t*)&udpDataGramLen + 2), &(clientaddr.sin_port), sizeof(clientaddr.sin_port));
    NfRelayThread::singleton()->enqueMsg(packet, udpDataGramLen, clientaddr.sin_addr.s_addr);
  }
}
