// use
#include "Thread.h"
#include "../log/CurrentThread.h"

#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <iostream>

namespace ccy {

namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";
}

pid_t getTid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cachedTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = getTid();
    t_tidStringLength =
        snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
  }
}
// 便在类的成员函数中使用 shared_from_this() 获得指向当前对象的 std::shared_ptr
class ThreadData : public std::enable_shared_from_this<ThreadData> {
public:
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid,
             CountDownLatch* latch)
      : _func(func), _name(name), _tid(tid), _latch(latch) {}

  void runInThread() {
    *_tid = CurrentThread::tid();
    _tid = nullptr;
    _latch->countDown();
    _latch = nullptr;

    CurrentThread::t_threadName = _name.empty() ? "Thread" : _name.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);

    _func();
    CurrentThread::t_threadName = "finished";
  }

private:
  ThreadFunc _func;
  std::string _name;
  pid_t* _tid;
  CountDownLatch* _latch;
};

void* startThread(void* obj) {
    std::shared_ptr<ThreadData> data = std::static_pointer_cast<ThreadData>(
        std::shared_ptr<ThreadData>(static_cast<ThreadData*>(obj),
                                  [](ThreadData*) {}));
    // Custom deleter to prevent double deletion

    // ThreadData* data = static_cast<ThreadData*>(obj);
    // data->runInThread();
    // delete data;
    // return nullptr;
    data->runInThread();
    return nullptr;
}

Thread::Thread(const ThreadFunc& func, const std::string& n)
    : _started(false),
      _joined(false),
      _pthreadId(0),
      _tid(0),
      _func(func),
      _name(n),
      _latch(1) {
  setDefaultName();
}

Thread::~Thread() {
  if (_started && !_joined)
    pthread_detach(_pthreadId);
}

void Thread::setDefaultName() {
  if (_name.empty()) {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread");
    _name = buf;
  }
}

void Thread::start() {
  assert(!_started);
  _started = true;
  auto data = std::make_shared<ThreadData>(_func, _name, &_tid, &_latch);

  if (pthread_create(&_pthreadId, nullptr, &startThread,
                     static_cast<void*>(data.get()))) {
    _started = false;
  } else {
    _latch.wait();
    assert(_tid > 0);
  }
}

int Thread::join() {
  assert(_started);
  assert(!_joined);

  _joined = true;
  return pthread_join(_pthreadId, nullptr);
}

}  // namespace ccy
