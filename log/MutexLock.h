#pragma once
#include<pthread.h>
#include<cstdio>
#include"noncopyable.h"

namespace ccy{

class MutexLock: noncopyable{
public:
    MutexLock() {pthread_mutex_init(&_mutex, nullptr);}
    ~MutexLock(){
        // pthread_mutex_lock(&_mutex);
        pthread_mutex_destroy(&_mutex);
    }
    void lock() {pthread_mutex_lock(&_mutex);}
    void unlock() {pthread_mutex_unlock(&_mutex);}

    pthread_mutex_t* get(){ return &_mutex;}


private:
    pthread_mutex_t _mutex;

private:
    friend class Condition;
};


class MutexLockGuard: noncopyable{
public:
    explicit MutexLockGuard(MutexLock &mutex) : _mutex{mutex}{_mutex.lock();}
    ~MutexLockGuard(){ _mutex.unlock();}
private:
    MutexLock &_mutex;
};

}