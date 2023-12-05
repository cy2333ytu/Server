#pragma once

#include"Channel.h"
#include"Epoll.h"
#include"./utils/CurrentThread.h"
#include"./utils/Thread.h"
#include"./log/Logging.h"
#include"./utils/utils.h"

#include<functional>
#include<memory>
#include<vector>

namespace ccy{

class EventLoop{
public:
    typedef std::function<void()> Func;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Func&& cb);
    void queueInLoop(Func&& cb);
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
    void assertInLoop() {assert(isInLoopThread());}

    void shutDown(ChannelPtr channel){shutDownWR(channel->getFd());}

    void removeFromPoller(ChannelPtr channel){poller_->epollDel(channel);}
    void updatePoller(ChannelPtr channel, int timeOut = 0) {poller_->epollMod(channel, timeOut);}
    void addPoller(ChannelPtr channel, int timeOut = 0) {poller_->epollAdd(channel, timeOut);}

private:
    bool looping_;
    std::shared_ptr<Epoll> poller_;
    int wakeupFd_;
    bool quit_;
    bool eventHandling_;
    mutable MutexLock mutex_;
    std::vector<Func> pendingFunctors_;
    bool callingPendingFunctors_;
    const pid_t threadId_;
    ChannelPtr pwakeUpChannel_;

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};

}