#include"EventLoopThreadPool.h"

namespace ccy{
EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int numThreads)
    : baseLoop_(baseLoop)
    , started_(false)
    , numThreads_(numThreads)
    , next_(0)
    {
        if(numThreads_ <= 0){
            LOG << "numThreads_ <= 0";
            abort();
        }
    }

void EventLoopThreadPool::start(){
    baseLoop_->assertInLoop();
    started_ = true;
    for(int i = 0; i < numThreads_; ++i){
        std::shared_ptr<EventLoopThread> thread(new EventLoopThread());
        threads_.push_back(thread);
        loops_.push_back(thread->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop(){
    baseLoop_->assertInLoop();
    assert(started_);
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        next_ = (next_+1) % numThreads_;
    }
    return loop;
}

}