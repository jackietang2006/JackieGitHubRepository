#ifndef NFDECODETHREAD_H
#define NFDECODETHREAD_H

#include "Thread.h"

class NfDecodeThread: public CThread
{
public:
    /**
      *   virtual destructor
      */
    virtual ~NfDecodeThread();

    /**
      *   Virtual function must be implemented here
      */
    virtual void Run();
    virtual void processNetFlow(FIFO<tuple<const char*, int, int>>::Node*) {}

    void sharedMemoryKey(key_t k) { mSharedMemoryKey = k; }
    key_t sharedMemoryKey() { return mSharedMemoryKey; }

    tuple<const char*, int, int>& getMsgQueueTail() { return mMsgQueueTail; }
    int&                          getNumOfMsgsDecoded() { return mNumOfMsgsDecoded; }

    short& decoderBufferPoolSize() { return mBufferPool.size(); }
    short& decoderMsgQueueSize() { return mMsgQueue.size(); }

    void enqueMsg(const char*, int, int);

protected:
    /**
      *   Default Constructor for thread
      */
    NfDecodeThread();

private:
    key_t       mSharedMemoryKey;
    int         mNumOfMsgsDecoded;
    tuple<const char*, int, int> mMsgQueueTail;
    FIFO<char*> mBufferPool;
    FIFO<tuple<const char*, int, int>> mMsgQueue;
};

class NfAnalyzerThread: public NfDecodeThread
{
public:
    static NfAnalyzerThread* singleton();
    /**
      *   Virtual function must be implemented here
      */
    virtual void processNetFlow(FIFO<tuple<const char*, int, int>>::Node*);
private:
    /**
      *   Default Constructor for thread
      */
    NfAnalyzerThread();

    static NfAnalyzerThread* mInstance;
};

class NfRelayThread: public NfDecodeThread
{
public:
    static NfRelayThread* singleton();
    void setRelayEnd(u_int32_t IP, u_int16_t Port);
    /**
      *   Virtual function must be implemented here
      */
    virtual void processNetFlow(FIFO<tuple<const char*, int, int>>::Node*);
private:
    /**
      *   Default Constructor for thread
      */
    NfRelayThread();

    static NfRelayThread* mInstance;
    int mSockfd;
    struct sockaddr_in mServAddr;
    u_int32_t mRelayIP;
    u_int16_t mRelayPort;
};

class NfMonitorThread: public CThread
{
public:
    static NfMonitorThread* singleton();
    void setThreadToBeMonitored(NfDecodeThread* aThread) { mThreadToBeMonitored = aThread; }
    /**
      *   Virtual function must be implemented here
      */
    virtual void Run();
private:
    static NfMonitorThread* mInstance;
    NfDecodeThread* mThreadToBeMonitored;
};

#endif
