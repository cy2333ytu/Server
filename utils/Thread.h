#pragma once
#include"../log/CountDownLatch.h"
#include"noncopyable.h"

#include<pthread.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<functional>
#include<memory>
#include<string>

namespace ccy{

class Thread: noncopyable{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();

    void start();
    int join();
    bool started() {return _started;}
    pid_t tid() const {return _tid;}
    const std::string& name() const {return _name;}

private:
    void setDefaultName();
    bool _started;
    bool _joined;
    pthread_t _pthreadId;
    pid_t _tid;
    ThreadFunc _func;
    std::string _name;
    CountDownLatch _latch;
};

}