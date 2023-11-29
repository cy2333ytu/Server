#pragma once
#include"Condition.h"
#include"../utils/MutexLock.h"
#include"../utils/noncopyable.h"

namespace ccy{

class CountDownLatch: noncopyable{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

private:
    mutable MutexLock _mutex;
    Condition _condition;
    int _count;
};
}