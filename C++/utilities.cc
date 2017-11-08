#include "utilities.h"
 
/*
 * error - wrapper for perror
 */
void error(const char *msg) {
  perror(msg);
  exit(1);
}

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

SharedMemory::SharedMemory(key_t key, int size, ShmType t) : mKey(key), mShmSize(size), mType(t)
{
  /*
   * Create the segment.
   */
  if (mType == SERVER && (mShmID = shmget(mKey, mShmSize, IPC_CREAT | 0666)) < 0) {
    error("shmget failed in creation...");
  }
  else if (mType == CLIENT && (mShmID = shmget(mKey, mShmSize, 0666)) < 0) {
    error("shmget failed in reference...");
  }

  /*
   * Now we attach the segment to our data space.
   */
  if ((mShm = (const char*)shmat(mShmID, NULL, 0)) == (char *) -1) {
      error("shmat");
  }
}

void SharedMemory::putShm(const void* cpyShmFrom)
{
  memcpy((void*)mShm, cpyShmFrom, mShmSize);
}
