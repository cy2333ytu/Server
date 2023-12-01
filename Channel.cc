#include"Channel.h"
#include"EventLoop.h"

namespace ccy{

    Channel::Channel(EventLoop* loop)
        :loop_(loop), events_(0), lastEvents_(0), fd_(0){}

    Channel::Channel(EventLoop* loop, int fd)
        :loop_(loop), fd_(fd), events_(0), lastEvents_(0){}
   
    Channel::~Channel(){}

    int Channel::getFd() const{return fd_;}
    int Channel::setFd(int fd){ fd_ = fd;}

    void Channel::handleEvents(){
        events_ = 0;
        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
            events_ = 0;
            return;
        }
        if(revents_ & EPOLLERR){
            if(errorHandler_) errorHandler_();
            events_ = 0;
            return;
        }
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP)){
            handleRead();
        }
        if(revents_ & EPOLLOUT){
            handleWrite();
        }
        handleConn();
    }

    void Channel::handleRead(){
        if(readHandler_){
            readHandler_();
        }
    }
    void Channel::handleWrite(){
        if(writeHandler_){
            writeHandler_();
        }   
    }
    void Channel::handleConn(){
        if(connHandler_){
            connHandler_();
        }
    }
}