#include"EventLoop.h"
#include"./utils/utils.cc"
#include"./log/Logging.h"

#include<sys/epoll.h>
#include<sys/eventfd.h>


namespace ccy{

__thread EventLoop* t_loopInThisThread = 0;

int createEventFd(){
    int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(event_fd < 0){
        LOG << "Error in eventfd";
        abort();
    }
    return event_fd;
}

EventLoop::EventLoop()
    : looping_(false)
    , poller_(new Epoll())
    , wakeupFd_(createEventFd())
    , quit_(false)
    , eventHandling_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , pwakeUpChannel_(new Channel(this, wakeupFd_)){
        if(t_loopInThisThread){
            LOG << "Another EventLoop " << t_loopInThisThread << 
                " exists in this thread " << threadId_;
        }else{
            t_loopInThisThread = this;
        }
        pwakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
        pwakeUpChannel_->setReadHandler(std::bind(&EventLoop::handleRead, this));
        pwakeUpChannel_->setConnHandler(std::bind(&EventLoop::handleConn, this));
        poller_->epollAdd(pwakeUpChannel_, 0);
    }

    void EventLoop::handleConn(){
        updatePoller(pwakeUpChannel_, 0);
    }

    EventLoop::~EventLoop(){
        close(wakeupFd_);
        t_loopInThisThread = nullptr;
    }

    void EventLoop::wakeup(){
        uint64_t one = 1;
        ssize_t n = writeN(wakeupFd_, (char*)(&one), sizeof one);
        if(n != sizeof one){
            LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::handleRead(){
        uint64_t one = 1;
        ssize_t n = readN(wakeupFd_, (char*)(&one), sizeof one);
        if(n != sizeof one){
            LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
        pwakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
    }
    
    void EventLoop::runInLoop(Func&& cb){
        if(isInLoopThread()) cb();
        else queueInLoop(std::move(cb));
    }

    void EventLoop::queueInLoop(Func&& cb){
        {
            MutexLockGuard lock(mutex_);
            pendingFunctors_.emplace_back(std::move(cb));
        }
        if(!isInLoopThread() || callingPendingFunctors_) wakeup();
    }

    void EventLoop::loop(){
        assert(!looping_);
        assert(isInLoopThread());
        looping_ = true;
        quit_ = false;

        std::vector<ChannelPtr> ret;
        while(!quit_){
            ret.clear();
            ret = poller_->poll();
            eventHandling_ = true;

            for(auto& it :ret) it->handleEvents();
            eventHandling_ = false;
            doPendingFunctors();
            poller_->handleExpired();
        }
        looping_ = false;
    }

    void EventLoop::doPendingFunctors(){
        std::vector<Func> functors;
        callingPendingFunctors_ = true;
        {
            MutexLockGuard lock(mutex_);
            functors.swap(pendingFunctors_);
        }
        for(size_t i = 0; i < functors.size(); i++) functors[i]();
        callingPendingFunctors_ = false;
    }
    
    void EventLoop::quit(){
        quit_ = true;
        if(!isInLoopThread()){
            wakeup();
        }
    }
}