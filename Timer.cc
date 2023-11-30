#include"Timer.h"
#include"../log/Logging.h"

#include<sys/time.h>
#include<unistd.h>
#include<queue>


namespace ccy{

TimeNode::TimeNode(std::shared_ptr<HttpData> requestData, int timeOut)
    :deleted_(false), HttpDataPtr(requestData)
{
    Logger::setLogFileName("../logFile/logTimer.log");
    struct timeval now;
    int ret = gettimeofday(&now, nullptr);
    
    if(ret == 0){
        expiredTime_ =
      (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeOut;
    }else{
        char buffer[256];
        strerror_r(errno, buffer, sizeof(buffer));  // Use strerror_r to make it thread-safe
        ccy::LOG << "Error in gettimeofday. Error code: " << errno << ", Error message: " << buffer;
    }
}

TimeNode::~TimeNode(){
    if(HttpDataPtr) HttpDataPtr->handleClose();
}
TimeNode::TimeNode(TimeNode &n)
    :HttpDataPtr(n.HttpDataPtr), expiredTime_(0){}

void TimeNode::update(int timeOut){
    struct timeval now;
    int ret = gettimeofday(&now, nullptr);
    
    if(ret == 0){
        expiredTime_ =
      (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeOut;
    }else{
        char buffer[256];
        strerror_r(errno, buffer, sizeof(buffer));  // Use strerror_r to make it thread-safe
        ccy::LOG << "Error in gettimeofday. Error code: " << errno << ", Error message: " << buffer;
    }
}

bool TimeNode::isValid(){
    struct timeval now;
    int ret = gettimeofday(&now, nullptr);
    
    if(ret == 0){
        size_t time = 
        (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));

        if(time < expiredTime_){
            return true;
        }
        else{
            this->setDelted();
            return false;
        }

    }else{
        char buffer[256];
        strerror_r(errno, buffer, sizeof(buffer));  // Use strerror_r to make it thread-safe
        ccy::LOG << "Error in gettimeofday. Error code: " << errno << ", Error message: " << buffer;
    }
}

void TimeNode::clearReq(){
    HttpDataPtr.reset();
    this->setDelted();
}

TimeManager::TimeManager(){}

TimeManager::~TimeManager(){}

void TimeManager::addTimer(std::shared_ptr<HttpData> HttpDataPtr, int timeOut){
    TimeNodePtr anotherNode(new TimeNode(HttpDataPtr, timeOut));
    timerNodeQueue.push(anotherNode);
    HttpDataPtr->linkTimer(anotherNode);
}
void TimeManager::handleExpiredEvent(){
    while(!timerNodeQueue.empty()){
        TimeNodePtr timerNowPtr = timerNodeQueue.top();
        if(timerNowPtr->isDeleted()){
            timerNodeQueue.pop();
        }else if(timerNowPtr->isValid() == false){
            timerNodeQueue.pop();
        }else break;
    }
}

}