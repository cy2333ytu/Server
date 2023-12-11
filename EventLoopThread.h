#pragma once
#include"EventLoop.h"
#include"./log/Condition.h"
#include"./utils/MutexLock.h"
#include"./utils/Thread.h"
#include"./utils/noncopyable.h"

namespace ccy{

class EventLoopThread: noncopyable{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    EventLoop* loop_;
    bool existing_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};

}
