#include"Epoll.h"
#include"./log/Logging.h"

#include<errno.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<memory>

namespace ccy{

const int EVENTSTOTAL = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll()
    :epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSTOTAL){}

Epoll::~Epoll() {}

void Epoll::epollAdd(ChannelPtr req, int timeOut){
    int fd = req->getFd();
    if(timeOut > 0){
        addTimer(req, timeOut);
        fd2Http_[fd] = req->getHolder();
    }
    
    struct epoll_event event;
    event.data.fd = fd;
    event.events = req->getEvents();
    req->EqualupdateLastEvents();

    fd2Chan_[fd] = req;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0){
        LOG <<  "Error adding fd to epoll: " << strerror(errno);
        fd2Chan_[fd].reset();
    }
}

void Epoll::epollMod(ChannelPtr req, int timeOut){
    if(timeOut > 0) addTimer(req, timeOut);
    int fd = req->getFd();
    if(!req->EqualupdateLastEvents()){
        struct epoll_event event;
        event.data.fd = fd;
        event.events = req->getEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0){
        LOG <<  "Error modify fd to epoll: " << strerror(errno);
        fd2Chan_[fd].reset();
    }   
    }
}

void Epoll::epollDel(ChannelPtr req){
    int fd = req->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = req->getEvents();

    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0){
        LOG <<  "Error delete fd to epoll: " << strerror(errno);
        fd2Chan_[fd].reset();
        fd2Http_[fd].reset();
    }
}

void Epoll::handleExpired(){timerManager_.handleExpiredEvent();}

std::vector<ChannelPtr> Epoll::poll(){
    while(true){
        int eventCount = 
            epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if(eventCount < 0)  LOG <<  "epoll wait error " << strerror(errno);
        std::vector<ChannelPtr> reqData = getEventsReq(eventCount);
        if(reqData.size() > 0) return reqData;
    }
}

std::vector<ChannelPtr> Epoll::getEventsReq(int eventNum){
    std::vector<ChannelPtr> reqData;
    for(int i = 0; i < eventNum; ++i){
        int fd = events_[i].data.fd;
        ChannelPtr curReq = fd2Chan_[fd];
        
        if(curReq){
            curReq->setRevents(events_[i].events);
            curReq->setEvents(0);
            reqData.emplace_back(curReq);
        }else{
            LOG << "Ptr curReq is invalid";
        }
    }
    return reqData;
}

void Epoll::addTimer(ChannelPtr reqData, int timeOut){
    std::shared_ptr<HttpData> time = reqData->getHolder();
    if(time) timerManager_.addTimer(time, timeOut);
    else LOG << "add timer fail";
}

}