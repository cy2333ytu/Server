#pragma once
#include"Channel.h"
#include"HttpData.h"
#include"Timer.h"

#include<memory>
#include<unordered_map>
#include<vector>
#include<sys/epoll.h>

namespace ccy{

class Epoll{
public:
    Epoll();
    ~Epoll();

    void epollAdd(ChannelPtr req, int timeOut);
    void epollMod(ChannelPtr req, int timeOut);
    void epollDel(ChannelPtr req);
    void handleExpired();

    std::vector<ChannelPtr> poll();
    std::vector<ChannelPtr> getEventsReq(int eventNum);

    void addTimer(ChannelPtr reqData, int timeOut);
    int getEpollFd() {return epollFd_;}

private:
    static const int MAXFDS = 100000;
    int epollFd_;
    
    TimeManager timerManager_;
    
    std::vector<epoll_event> events_;
    ChannelPtr fd2Chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2Http_[MAXFDS];
};

}