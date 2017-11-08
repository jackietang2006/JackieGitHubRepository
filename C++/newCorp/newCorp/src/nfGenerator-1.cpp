/*
 * Usage: nfGenerator <IP address> <Port>
 *
 */

/*
 * header files for UDP socket
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <sstream>
#include <dirent.h>
#include "netflow_v9.h"
#include "netflowV9Container.h"
#include "routerTemplateContainer.h"
#include "networkV9TemplateContainer.h"

using namespace std;
using std::cout;
using std::endl;
using std::ifstream;

FILE* inputUdpPacketsFB;

/*
 * error - wrapper for perror
 */
void error(const char *msg) {
  perror(msg);
  exit(1);
}

/*
    96 bit (12 bytes) pseudo header needed for udp header checksum calculation
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

/*
    Generic checksum calculation function
    Calculating checksum in network mode
*/
unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

// global variables
networkV9TemplateContainer* ntContainer;

char taddr[MAX_NAME_LEN];
u_int16_t  fId[40], fValue[40];

int sockfd;
struct sockaddr_in servaddr;

//------------file sorting function--------------
static bool fileSort (char* aName, char* bName)
{
    return isDecending (aName, bName);
}

std::string get_env_var( std::string const & key )
{
  char * val;
  val = getenv( key.c_str() );
  std::string retval = "";
  if (val != NULL) {
    retval = val;
  }
  return retval;
}

void netFlowExport(char* inDir, char* inFileName)
{
  static int totalPcapRecords = 0;
  static int totalNfRecords = 0;
  int udpHdrLen = 0, maxUdpHdrLen = 0;

  char fullName[MAX_CHARS_PER_LINE];
  char errbuf[PCAP_ERRBUF_SIZE];
  const u_char *packet;
  struct pcap_pkthdr header;
  int n1;

  strcpy (fullName, inDir);
  strcat (fullName, "/");
  strcat (fullName, inFileName);
  cout << "Processing pcap file: " << fullName << endl;

  pcap_t* pcapFB = pcap_open_offline(fullName, errbuf);
  if (pcapFB == NULL)
  {
      cout << "failed to open pcap file" << endl;
      return;
  }

  // start reading pcap packets
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 25000;

  while (0 != (packet = pcap_next(pcapFB, &header)))
  {
    totalPcapRecords ++;

    ether_header *eh = (ether_header*)packet;
    uint16_t ether_type = ntohs(eh->ether_type);

    //ETH frame
    u_char *pkt_ptr = (u_char *)packet;
    int ether_offset = 0;
    if (ether_type != ETHERTYPE_IP)
      continue;

    ether_offset = 14;

    //IP packet
    pkt_ptr += ether_offset;  //skip past the Ethernet II header
    struct iphdr *ip_hdr = (struct iphdr *)pkt_ptr; //point to an IP header structure

    // CFLOW transfered via UDP
    if(ip_hdr->protocol != IPPROTO_UDP)
      continue;

    //UDP datagram
    pkt_ptr += sizeof(iphdr);
    udphdr *udp_hdr = (struct udphdr*)pkt_ptr;
    udpHdrLen = ntohs(udp_hdr->len);
    maxUdpHdrLen = (maxUdpHdrLen < udpHdrLen) ? udpHdrLen : maxUdpHdrLen;

    //UDP payload
    pkt_ptr += sizeof(udphdr);

    // IP checksum verification
    unsigned short iphChecksum = csum ((unsigned short *) ip_hdr, sizeof(struct iphdr));
    if (iphChecksum != 0)
    {
      fprintf(inputUdpPacketsFB, "UDP datagram#%d#%d (%4d): ipsum(0x%4X), udpsum(NA)\n", totalPcapRecords, totalNfRecords, ntohs(udp_hdr->len) - 8, ntohs(iphChecksum), 0);
      continue;
    }

    // UDP checksum verification
    struct pseudo_header psh;
    psh.source_address = ip_hdr->saddr;
    psh.dest_address = ip_hdr->daddr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = udp_hdr->len;

    int psize = sizeof(struct pseudo_header) + udpHdrLen;
    unsigned char* pseudogram = (unsigned char*)malloc(psize);

    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , udp_hdr , udpHdrLen);

    unsigned short udphChecksum = csum( (unsigned short*) pseudogram , psize);
    if (udphChecksum != 0)
    {
      fprintf(inputUdpPacketsFB, "UDP datagram#%d#%d (%4d): ipsum(0x%4X), udpsum(0x%4X)\n", totalPcapRecords, totalNfRecords, ntohs(udp_hdr->len) - 8, ntohs(iphChecksum), ntohs(udphChecksum));
      continue;
    }

    totalNfRecords ++;
    sendto(sockfd, pkt_ptr, ntohs(udp_hdr->len) - 8, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    fprintf(inputUdpPacketsFB, "UDP datagram#%d#%d (%4d): ipsum(0x%4X), udpsum(0x%4X)\n", totalPcapRecords, totalNfRecords, ntohs(udp_hdr->len) - 8, ntohs(iphChecksum), ntohs(udphChecksum));
    /*
     * print UDP datagram in hex format
     */
    #define BYTETOHEX "%02X "
    //for(int i = 0; i < ntohs(udp_hdr->len) - 8; i++)
    //{
    //  fprintf(inputUdpPacketsFB, BYTETOHEX, pkt_ptr[i] & 0xFF);
    //}
    //fprintf(inputUdpPacketsFB, "\n");
    fflush(inputUdpPacketsFB);

    if(nanosleep(&tim , &tim2) < 0 )
    {
      error((char*)"Nano sleep system call failed!\n");
    }
  } // end pcap file reading
  fprintf(inputUdpPacketsFB, "Max UDP Hdr Len: %d\n", maxUdpHdrLen);
  pcap_close(pcapFB);
  pcapFB=NULL;
}


//-----------------main program-------------------
int main(int argc, char** argv)
{
   char sendline[1000];
   char recvline[1000];
   struct hostent *hostp;

   if (argc != 3)
   {
      printf("usage:  nfGenerator <IP address> <Port>\n");
      exit(1);
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);
   if (sockfd < 0)
    error((char*)"ERROR opening socket");

   int sndbuf = 62464;
   int rcvbuf = 62464;
   //if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&sndbuf, sizeof(int)) < 0)
   //  error((char*)"ERROR setsockopt: SO_SNDBUF");
   //if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&rcvbuf, sizeof(int)) < 0)
   //  error((char*)"ERROR setsockopt: SO_RCVBUF");

   /*
    * getsockopt: Query UDP socket's SNDBUF and RCVBUF
    */
   sndbuf = 0;
   rcvbuf = 0;
   int optlen = sizeof sndbuf;
   if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *)&sndbuf, (socklen_t*)&optlen) < 0)
     error((char*)"ERROR getsockopt: SO_SNDBUF");
   if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&rcvbuf, (socklen_t*)&optlen) < 0)
     error((char*)"ERROR getsockopt: SO_RCVBUF");
   printf("Querying socket SNDBUF = %d, RCVBUF = %d\n", sndbuf, rcvbuf);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port=htons(atoi(argv[2]));

   if((servaddr.sin_addr.s_addr=inet_addr(argv[1])) == (unsigned long)INADDR_NONE)
   {

     /* When passing the host name of the server as a */
     /* parameter to this program, use the gethostbyname() */
     /* function to retrieve the address of the host server. */
     /***************************************************/
     /* get host address */
     hostp = gethostbyname(argv[1]);
     if(hostp == (struct hostent *)NULL)
     {
       printf("HOST NOT FOUND --> ");
       /* h_errno is usually defined */
       /* in netdb.h */
       printf("h_errno = %d\n",h_errno);
       printf("---This is a client program---\n");
       printf("Command usage: %s <server name or IP>\n", argv[0]);
       close(sockfd);
       exit(-1);
     }
     memcpy(&servaddr.sin_addr, hostp->h_addr, sizeof(servaddr.sin_addr));
   }

   /*
   while (fgets(sendline, 10000,stdin) != NULL)
   {
      //
      //Trimming off the ending '\n'
      //
      n = strlen(sendline) - 1;
      sendline[n] = 0;
      sendto(sockfd,sendline,n,0, (struct sockaddr *)&servaddr,sizeof(servaddr));
      //n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      //recvline[n]=0;
      //fputs(recvline,stdout);
      //fputc('\n',stdout);
   }
   */
  
  // check arguments
  char nfPcap[MAX_CHARS_PER_LINE] ;
  char nfCtrl[MAX_CHARS_PER_LINE] ;

  int nDirectory = 0;
  int nFile = 0;
  int n1 = 0;
  
  //-----------------------------------------
  // open file descripters for error and reports
  // netflow pcap directory
  string DataDir = get_env_var((char*)"NetFlowDataDir");
  if (DataDir.empty())
  {
    error("ENV Variable - NetFlowDataDir: not defined yet!");
  }
  cout << "ENV Variable - NetFlowDataDir: " << DataDir << endl;

  strcpy (nfPcap, DataDir.c_str());
  strcat (nfPcap,"/pcap");
  // netflow template
  strcpy (nfCtrl, DataDir.c_str());
  strcat (nfCtrl,"/control");

  // open file descripters to write UDP packets
  char inputUdpPackets[MAX_CHARS_PER_LINE];
  strcpy (inputUdpPackets, DataDir.c_str());
  strcat (inputUdpPackets,"/log/inputUdpPackets.log");
  inputUdpPacketsFB=fopen(inputUdpPackets, "w");
  if (inputUdpPacketsFB == NULL)
  {
    printf("fail to open log/inputUdpPackeets.log, check the file priviledge\n");
    return 0;
  }

  //-------------start process here--------------
  string in;
  char input[MAX_CHARS_PER_LINE] = "";
  int ninput;
  ntContainer = new networkV9TemplateContainer();

  struct dirent *dirp;
  DIR *dp = opendir(nfPcap);
  if (dp == NULL) return 0;

  vector <char*> fileContainer;
  while ((dirp = readdir( dp )))
  {
    if (compareString(dirp->d_name,".")==0 || compareString(dirp->d_name,"..")==0) continue;
      char * filename = new char[48];
    strcpy (filename, dirp->d_name);
    fileContainer.push_back (filename);
  }

  // sort files to decending order
  std::stable_sort (fileContainer.begin(),fileContainer.end(),fileSort);

  for (int f1=0; f1< fileContainer.size(); f1++)
  {
    cout << "Scan Netflow file: "<<fileContainer[f1]<< endl;
    netFlowExport(nfPcap, fileContainer[f1]);
  }

/*
  // wait for SNDBUF to clear
  struct timespec tim, tim2;
  tim.tv_sec = 3600;
  tim.tv_nsec = 999999999;
  if(nanosleep(&tim , &tim2) < 0 )
  {
    error((char*)"Nano sleep system call failed!\n");
  }
*/
}
