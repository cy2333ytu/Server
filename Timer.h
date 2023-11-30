#pragma once
#include"../utils/MutexLock.h"
#include"../utils/noncopyable.h"
#include"HttpData.h"

#include<deque>
#include<memory>
#include<queue>
#include<unistd.h>
#include<functional>

namespace ccy{

class HttpData;

class TimeNode{
public: 
    TimeNode(std::shared_ptr<HttpData> requestData, int timeOut);
    ~TimeNode();
    TimeNode(TimeNode &n);

    void update(int timeOut);
    void clearReq();
    void setDelted() {deleted_ = true;}
    bool isValid();
    bool isDeleted() const {return deleted_;}
    size_t getExpTime() const {return expiredTime_;}

private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> HttpDataPtr;
};

// class TimeCmp{
// public:
//     bool operator()(std::shared_ptr<TimeNode> &first,
//                   std::shared_ptr<TimeNode> &second) const
//                   {
//                     return first->getExpTime() > second->getExpTime();
//                   }
// };

class TimeManager{
public:
    TimeManager();
    ~TimeManager();
    void addTimer(std::shared_ptr<HttpData> HttpDataPtr, int timeOut);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimeNode> TimeNodePtr;
    std::function<bool(const TimeNodePtr&, const TimeNodePtr&)>
        cmp = [](const TimeNodePtr& a, const TimeNodePtr& b) {
            return a->getExpTime() > b->getExpTime();
        };
    std::priority_queue<TimeNodePtr, std::deque<TimeNodePtr>, 
            decltype(cmp)> timerNodeQueue{cmp};
    
    // In the context of std::priority_queue, 
    // std::priority_queue<TimeNodePtr, std::deque<TimeNodePtr>,
    //     TimeCmp>    timerNodeQueue;
    // the third template parameter is the type of the comparator, 
    // and it requires a specific type. Since auto doesn't represent a 
    // specific type that the compiler can deduce, it cannot be used in this case.

};

}

