#pragma once

#include "Common.h"
#include "FixedBuffer.h"
#include "Queue.h"

namespace dm {

class Producer
{
private:
    const String    &mSrcString;

    const u32       mCount;

    const u32       mBufferCapacity;
    FixedBufferPtr  mCurrentData = nullptr;

    FixedBufferPtr  mReadyData = nullptr;
    Semaphore       mConsumeSem;

    // queue
    Queue           &mQueue;

public:
    Producer(const u32 count, const String &src, const u32 buffer_cap, Queue &queue);

    ~Producer()
    {
        mConsumeSem.wait();
    }

public:
    void    produce();

private:
    void    generateNextString();

    void    swapBuffer();
};


class Writer
{
private:
    FILE    *mFile = nullptr;

    Queue   &mQueue;

public:
    Writer(FILE *file, Queue &queue)
    :
    mFile(file),
    mQueue(queue)
    {

    }

public:
    bool write();
};

}   // end of namespace dm
