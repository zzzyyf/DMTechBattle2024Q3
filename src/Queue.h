#pragma once
#include "Common.h"
#include "FixedBuffer.h"
#include "Semaphore.h"

#include <cerrno>
#include <mutex>
#include <queue>
#include <atomic>

namespace dm {

struct QueueElem
{
public:
    FixedBuffer &mData;

    Semaphore   &mNotifier;

public:
    QueueElem(FixedBuffer &buffer, Semaphore &notifier)
    :
    mData(buffer),
    mNotifier(notifier)
    {

    }
};

using QueueElemPtr = Ptr<QueueElem>;

class Queue
{
private:
    std::mutex                  mLock;
    std::queue<QueueElemPtr>    mQueue;

    Semaphore                   mReadSem;

    const u32                   mProducerCount;
    std::atomic<u32>            mEOF = false;

public:
    Queue(u32 n_producer = 1)
    :
    mProducerCount(n_producer)
    {

    }

    template <class... Args>
    void emplace_back(Args &&... args)
    {
        {
            std::scoped_lock lock(mLock);
            mQueue.emplace(std::make_shared<QueueElem>(std::forward<Args>(args)...));
        }
        bool rslt = mReadSem.post();
        assert(rslt);
    }

    QueueElemPtr    pop()
    {
        while (true)
        {
            bool timedout = false;
            bool rslt = mReadSem.timed_wait(1, timedout);
            assert(rslt);
            if (!timedout) {
                break;
            }
            if (mEOF == mProducerCount) {
                return nullptr;
            }
        }

        std::scoped_lock lock(mLock);
        assert(!mQueue.empty());
        QueueElemPtr ret = mQueue.front();
        mQueue.pop();
        return ret;
    }

    inline void setEOF()
    {
        mEOF++;
    }
};

}   // end of namespace dm
