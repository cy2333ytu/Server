#pragma once
#include"Timer.h"

#include<sys/epoll.h>
#include<memory>
#include<functional>
#include<unordered_map>
#include<string>

namespace ccy{

class EventLoop;
class HttpData;

class Channel{
public:
    typedef std::function<void()> CallBackFunc;
    
    Channel(EventLoop* loop);
    Channel(EventLoop* loop, int fd);
    ~Channel();
    int getFd() const;
    int setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder) {holder_ = holder;}
    std::shared_ptr<HttpData> getHolder(){
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    void setReadHandler(CallBackFunc &&readHandler){readHandler_ = readHandler;}
    void setWriteHandler(CallBackFunc &&writeHandler){writeHandler_ = writeHandler;}
    void seterrorHandler(CallBackFunc &&errorHandler){errorHandler_ = errorHandler;}
    void setConnHandler(CallBackFunc &&connHandler){connHandler_ = connHandler;}

    void handleEvents();
    void handleRead();
    void handleWrite();
    void handleError(int fd, int errNum, std::string shortMsg);
    void handleConn();

    void setRevents(__uint32_t ev){revents_ = ev;}
    __uint32_t &getEvents() {return events_;}

    bool updateLastEvents(){
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }
    __uint32_t getLastEvent() {return lastEvents_;}

private:
    EventLoop *loop_;
    int fd_;

    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;
    
    std::weak_ptr<HttpData> holder_;

private:
    int parse_URI();
    int parse_Headers();
    int analysis_Request();

    CallBackFunc readHandler_;
    CallBackFunc writeHandler_;
    CallBackFunc errorHandler_;
    CallBackFunc connHandler_;
};

typedef std::shared_ptr<Channel> ChannelPtr;
}
