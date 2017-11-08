#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utilities.h"
 
int main(int argc, char **argv)
{
  struct timespec tim, tim2;
  tim.tv_sec = 1;
  tim.tv_nsec = 0;

  /*
   * check command line arguments
   */
  if (argc != 2) {
    error("usage: nfMonitor <KEY> [info: KEY = 1111 for Collector & 2222 for RelayAgent]");
  }
  key_t shmKey = atoi(argv[1]);

  SharedMemory* shmClient = new SharedMemory(shmKey, SHAREDMEMORY_SIZE, SharedMemory::CLIENT);
  const char* shmBuf = NULL;
  const char* shmPtr = NULL;

  char msgBuf[NFLOWMSGSIZE];
  short msgLen = 0, clientport = 0;
  unsigned int clientaddr;
  short bufferPoolSize, msgQueueSize;
  int numOfMsgsDecoded;
  while(1)
  {
    if(nanosleep(&tim , &tim2) < 0 )
    {
      error((char*)"Nano sleep system call failed!\n");
    }

    shmBuf = shmClient->getShm();
    shmPtr = shmBuf;

    memset(msgBuf, 0, NFLOWMSGSIZE);
    memcpy(msgBuf, shmPtr, NFLOWMSGSIZE);

    shmPtr += NFLOWMSGSIZE;
    msgLen = *((short*)shmPtr);

    shmPtr += sizeof(short);
    clientport = *((short*)shmPtr);

    shmPtr += sizeof(short);
    clientaddr = *((int*)shmPtr);

    shmPtr += sizeof(int);
    bufferPoolSize = *((short*)shmPtr);

    shmPtr += sizeof(short);
    msgQueueSize = *((short*)shmPtr);

    shmPtr += sizeof(short);
    numOfMsgsDecoded = *((int*)shmPtr);

    /*
     * print UDP datagram in hex format
     */
    #define BYTETOHEX "%02X "
    printf("Received datagram (%d bytes) from sourece (%s:%d)\n", msgLen, inet_ntoa(*(struct in_addr*)&clientaddr), ntohs(clientport));
    for(int i = 0; i < msgLen; i++)
    {
      printf(BYTETOHEX, msgBuf[i] & 0xFF);
    }
    printf("\n");
    printf("***BufferPoolSize: %5d --- MsgQueueSize: %5d*** --- NumberOfMsgsDecoded: %u\n", bufferPoolSize, msgQueueSize, numOfMsgsDecoded);
    fflush(stdout);
  }
}
