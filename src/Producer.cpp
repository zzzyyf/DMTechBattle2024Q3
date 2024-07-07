#include "Producer.h"
#include "FixedBuffer.h"

#include <cstdio>
#include <cstring>
#include <sys/random.h>

namespace dm {

Producer::Producer(const u32 count, const String &src, const u32 buffer_cap, Queue &queue)
:
mSrcString(src),
mCount(count),
mBufferCapacity(buffer_cap),
mCurrentData(std::make_shared<FixedBuffer>(buffer_cap / 2)),
mReadyData(std::make_shared<FixedBuffer>(buffer_cap / 2)),
mConsumeSem(1),
mQueue(queue)
{

}


void
Producer::produce()
{
    for (u32 i = 0; i < mCount; i++)
    {
        generateNextString();
    }
    swapBuffer();
    mQueue.setEOF();
}


void
Producer::generateNextString()
{
    u16 len;
    i32 rslt = ::getrandom(&len, 2/*length*/, 0/*flags*/);
    assert(rslt == 2);
    len = len % 598 + 3;

    char *dest = mCurrentData->getData(len+1);
    if (!dest)
    {
        swapBuffer();
        dest = mCurrentData->getData(len+1);
        assert(dest);
    }

    writeLength(len, dest);
    ::memcpy(dest+2, mSrcString.data()+2, len-2);
    dest[len] = '\n';   // TODO change to '\0' after debug

    return;
}


void
Producer::swapBuffer()
{
    if (mCurrentData->getSize() == 0) {
        return;
    }
    mConsumeSem.wait();

    std::swap(mCurrentData, mReadyData);
    mCurrentData->clear();

    mQueue.emplace_back(*mReadyData, mConsumeSem);
}


bool
Writer::write()
{
    QueueElemPtr elem = nullptr;
    while (true)
    {
        elem = mQueue.pop();
        if (!elem) {
            return true;
        }

        const char *data = elem->mData.getData();
        u32 len = elem->mData.getSize();
        assert(len > 0);
        i32 nwrite = std::fwrite(data, len, 1, mFile);
        if (nwrite != 1)
        {
            std::cerr << "error write data len: " << len << " to file" << std::endl;
            return false;
        }

        elem->mData.clear();
        elem->mNotifier.post();
    }
}


}   // end of namespace ncdb
