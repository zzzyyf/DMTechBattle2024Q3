#pragma once
#include "Common.h"

#include <cerrno>
#include <ctime>
#include <semaphore.h>

namespace dm {

class Semaphore
{
private:
    sem_t   mSem;
    bool    mIsGood = false;

public:
    Semaphore(u32 value = 0)
    {
        i32 rslt = sem_init(&mSem, 0, value);
        if (rslt == 0) {
            mIsGood = true;
        }
    }

    Semaphore(const Semaphore &rhs) = delete;

    Semaphore(Semaphore &&rhs) = delete;

    Semaphore &operator=(const Semaphore &rhs) = delete;

    Semaphore &operator=(Semaphore &&rhs) = delete;

    ~Semaphore()
    {
        if (!mIsGood) {
            return;
        }
        sem_destroy(&mSem);
    }

    bool post()
    {
        if (!mIsGood) {
            return false;
        }

        i32 rslt = sem_post(&mSem);
        return rslt == 0;
    }

    bool wait()
    {
        if (!mIsGood) {
            return false;
        }

        i32 rslt = sem_wait(&mSem);
        return rslt == 0;
    }

    bool timed_wait(u32 timeout_ms, bool &timedout)
    {
        if (!mIsGood) {
            return false;
        }

        struct timespec tm;
        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += timeout_ms / 1000;
        tm.tv_nsec += timeout_ms % 1000 * 1000'000l;
        if (tm.tv_nsec >= 1000'000'000)
        {
            tm.tv_nsec -= 1000'000'000;
            tm.tv_sec++;
        }

        i32 rslt = sem_timedwait(&mSem, &tm);
        if (rslt == 0) {
            return true;
        }
        if (errno == ETIMEDOUT)
        {
            timedout = true;
            return true;
        }
        return false;
    }
};

}   // end of namespace dm
