#pragma once
#include "Common.h"

namespace dm {

class FixedBuffer
{
private:
    const u32   mCapacity = 0;
    char        *mData = nullptr;

    char        *mPos = nullptr;

public:
    FixedBuffer(u32 capacity)
    :
    mCapacity(capacity),
    mData(new char[capacity]),
    mPos(mData)
    {

    }

    ~FixedBuffer()
    {
        delete[] mData;
    }

    char*   getData(const u32 len)
    {
        assert(mPos && mData && mPos >= mData);

        if (mPos - mData + len > mCapacity) {
            return nullptr;
        }

        char *pos = mPos;
        mPos += len;
        return pos;
    }

    inline const char *getData() { return mData; }
    inline u32  getSize() const { return mPos - mData; }
    inline void clear() { mPos = mData; }
};

using FixedBufferPtr = Ptr<FixedBuffer>;

}   // end of namespace dm