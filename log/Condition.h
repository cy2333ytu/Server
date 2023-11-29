#pragma once
#include<errno.h>
#include<pthread.h>
#include<time.h>
#include<cstdint>
#include"MutexLock.h"
#include"noncopyable.h"

namespace ccy{

class Condition: noncopyable{
public:
    explicit Condition(MutexLock &mutex): _mutex(mutex){
        pthread_cond_init(&cond, nullptr);
    }
    
    ~Condition(){pthread_cond_destroy(&cond);}
    void wait(){pthread_cond_wait(&cond, _mutex.get());}
    
    void notify() {pthread_cond_signal(&cond);}
    void notifyAll() {pthread_cond_broadcast(&cond);}

    bool waitForSeconds(int seconds){
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, _mutex.get(), &abstime);
    }

private:
    MutexLock &_mutex;
    pthread_cond_t cond;
};

}