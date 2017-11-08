#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <parserCommon.h>
#include <NfDecodeThread.h>
#include <networkV9TemplateContainer.h>
#include <RawUdp.h>

// global variables
extern networkV9TemplateContainer* ntContainer;

extern char tempName[MAX_CHARS_PER_LINE];
extern FILE* tempFB;

NfDecodeThread::NfDecodeThread() : mSharedMemoryKey(0), mNumOfMsgsDecoded(0)
{
  char* nfBuffer;
  for(int i = 0; i < NFLOWBUFSIZE; i++)
  {
    nfBuffer = new char[NFLOWMSGSIZE];
    memset(nfBuffer, 0, NFLOWMSGSIZE);
    mBufferPool.enqueue(nfBuffer);
  }
}
 
NfDecodeThread::~NfDecodeThread()
{
}
 
void NfDecodeThread::enqueMsg(const char* aMsg, int lengthAndPortInNetworkOrder, int sourceIP)
{
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 100;

  {
    lock_guard<mutex> autoLock(mMutex);
    mNumOfMsgsDecoded ++;
  }

  FIFO<char*>::Node* aNodeFromBufferPool = NULL;
  while(1)
  {
    {
      lock_guard<mutex> autoLock(mMutex);
      aNodeFromBufferPool = mBufferPool.dequeue();
    }
    if (aNodeFromBufferPool) break;

    if(nanosleep(&tim , &tim2) < 0 )
    {
      error((char*)"Nano sleep system call failed!\n");
    }
  }

  short length = *(short*)&lengthAndPortInNetworkOrder;
  memcpy(aNodeFromBufferPool->value, aMsg, length);

  {
    lock_guard<mutex> autoLock(mMutex);
    std::get<0>(mMsgQueueTail) = aNodeFromBufferPool->value;
    std::get<1>(mMsgQueueTail) = lengthAndPortInNetworkOrder;
    std::get<2>(mMsgQueueTail) = sourceIP;
    mMsgQueue.enqueue(mMsgQueueTail);

    //release memory for BufferPool
    delete aNodeFromBufferPool;
  }
}

void NfDecodeThread::Run()
{
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 100;

  FIFO<tuple<const char*, int, int>>::Node* aNodeFromMsgQueue = NULL;
  while(1)
  {
    {
      lock_guard<mutex> autoLock(mMutex);
      aNodeFromMsgQueue = mMsgQueue.dequeue();
    }
    if (!aNodeFromMsgQueue)
    {
      if(nanosleep(&tim , &tim2) < 0 )
      {
        error((char*)"Nano sleep system call failed!\n");
      }
      continue;
    }

    processNetFlow(aNodeFromMsgQueue);

    /*
    // export template
    tempFB = fopen(tempName, "w");
    ntContainer->printTemplateAll(tempFB);
    fclose (tempFB);
    */

    {
      lock_guard<mutex> autoLock(mMutex);
      mBufferPool.enqueue((char*)std::get<0>(aNodeFromMsgQueue->value));

      //release memory for MsgQueue
      delete aNodeFromMsgQueue;
    }
  }
}

//NfAnalyzerThread
extern long processNF9Packet(unsigned char *buf, int len, unsigned int cliRouterIpAddr);

NfAnalyzerThread* NfAnalyzerThread::mInstance = NULL;

NfAnalyzerThread* NfAnalyzerThread::singleton()
{
  if(!mInstance)
    mInstance = new NfAnalyzerThread;
  return mInstance;
}

NfAnalyzerThread::NfAnalyzerThread()
{
}
 
void NfAnalyzerThread::processNetFlow(FIFO<tuple<const char*, int, int>>::Node* aNode)
{
  int lengthAndPortInNetworkOder = get<1>(aNode->value);
  short length = *(short*)&lengthAndPortInNetworkOder;
  processNF9Packet((unsigned char*)get<0>(aNode->value), length, get<2>(aNode->value));
}
 
//NfRelayThread
NfRelayThread* NfRelayThread::mInstance = NULL;

NfRelayThread* NfRelayThread::singleton()
{
  if(!mInstance)
    mInstance = new NfRelayThread;
  return mInstance;
}

NfRelayThread::NfRelayThread() : mRelayIP(0), mRelayPort(0)
{
  //Create a raw socket of type IPPROTO
  mSockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);

  if (mSockfd < 0)
    error((char*)"ERROR opening socket");
}
 
void NfRelayThread::setRelayEnd(u_int32_t IP, u_int16_t Port)
{
  mRelayIP = IP;
  mRelayPort = Port;
  bzero(&mServAddr,sizeof(struct sockaddr_in));

  mServAddr.sin_family = AF_INET;
  mServAddr.sin_addr.s_addr = mRelayIP;
  mServAddr.sin_port   = mRelayPort;
}
 
void NfRelayThread::processNetFlow(FIFO<tuple<const char*, int, int>>::Node* aNode)
{
  const u_char* nf_pkt    = (const u_char*)get<0>(aNode->value);
  int nf_pkt_lenANDport = get<1>(aNode->value);
  int nf_pkt_saddr      = get<2>(aNode->value);
  short nf_pkt_len = 0, nf_pkt_port = 0;
  memcpy(&nf_pkt_len, &nf_pkt_lenANDport, 2);
  memcpy(&nf_pkt_port, ((u_int8_t*)&nf_pkt_lenANDport + 2), 2);

  u_char rawUdpDG[NFLOWMSGSIZE];
  int rawUdpDG_len;

  //RawUdp per dest IP and Port
  RawUdp aRawUdp(mRelayIP, mRelayPort);

  aRawUdp.buildRawUdpDataGram(nf_pkt_saddr, nf_pkt_port, nf_pkt, nf_pkt_len, rawUdpDG, rawUdpDG_len, csum);
  sendto(mSockfd, rawUdpDG, ntohs(rawUdpDG_len), 0, (struct sockaddr *)&mServAddr, sizeof(struct sockaddr_in));
}
 
//NfMonitorThread
NfMonitorThread* NfMonitorThread::mInstance = NULL;

NfMonitorThread* NfMonitorThread::singleton()
{
  if(!mInstance)
    mInstance = new NfMonitorThread;
  return mInstance;
}

void NfMonitorThread::Run()
{
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 10000000;

  SharedMemory* shmServer = new SharedMemory(mThreadToBeMonitored->sharedMemoryKey(), SHAREDMEMORY_SIZE, SharedMemory::SERVER);
  char shmBuf[SHAREDMEMORY_SIZE];
  char* shmPtr = NULL;

  tuple<const char*, int, int>& theMsgQueueTail = mThreadToBeMonitored->getMsgQueueTail();

  while(1)
  {
    if(nanosleep(&tim , &tim2) < 0 )
    {
      error((char*)"Nano sleep system call failed!\n");
    }

    //Initializing shmBuf & shmPtr
    shmPtr = shmBuf;
    memset(shmPtr, 0, SHAREDMEMORY_SIZE);

    int lengthAndPortInNetworkOder = get<1>(theMsgQueueTail);
    short length = *(short*)&lengthAndPortInNetworkOder;
    memcpy(shmPtr, get<0>(theMsgQueueTail), length);
    shmPtr += NFLOWMSGSIZE;
    memcpy(shmPtr, &(get<1>(theMsgQueueTail)), sizeof(int));
    shmPtr += sizeof(int);
    memcpy(shmPtr, &(get<2>(theMsgQueueTail)), sizeof(int));
    shmPtr += sizeof(int);
    memcpy(shmPtr, &(mThreadToBeMonitored->decoderBufferPoolSize()), sizeof(short));
    shmPtr += sizeof(short);
    memcpy(shmPtr, &(mThreadToBeMonitored->decoderMsgQueueSize()), sizeof(short));
    shmPtr += sizeof(short);
    memcpy(shmPtr, &(mThreadToBeMonitored->getNumOfMsgsDecoded()), sizeof(int));

    shmServer->putShm((const void*)shmBuf);
  }
}
