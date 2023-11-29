#pragma once
#include"CountDownLatch.h"
#include"LogStream.h"
#include"../utils/MutexLock.h"
#include"../utils/Thread.h"
#include"../utils/noncopyable.h"

#include<string>
#include<vector>
#include<functional>

namespace ccy{

class AsyncLogging: noncopyable{
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging(){
        if(_running) stop();
    }
    void append(const char* logline, int len);
    
    void start(){
        _running = true;
        _thread.start();
        _latch.wait();
    }

    void stop(){
        _running = false;
        _cond.notify();
        _thread.join();
    }

private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    
    const int _flushInterval;
    bool _running;
    
    std::string _basename;
    Thread _thread;
    MutexLock _mutex;
    Condition _cond;
    BufferPtr _currentBuffer;
    BufferPtr _nextBuffer;
    BufferVector _buffers;
    CountDownLatch _latch;
};

}