#include"CountDownLatch.h"

namespace ccy{

CountDownLatch::CountDownLatch(int count)
    :_mutex(), _condition(_mutex), _count(count)
{

}

void CountDownLatch::wait(){
    MutexLockGuard lock(_mutex);
    while(_count > 0) _condition.wait();
}

void CountDownLatch::countDown(){
    MutexLockGuard lock(_mutex);
    --_count;
    if(_count == 0) _condition.notifyAll();
}


}