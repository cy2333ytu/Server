#include"AsyncLogging.h"
#include"LogFile.h"

#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<functional>

namespace ccy{

AsyncLogging::AsyncLogging(const std::string logFileName, int flushInterval)
    : _flushInterval(flushInterval)
    , _running(false)
    , _basename(logFileName)
    , _thread(std::bind(&AsyncLogging::threadFunc, this), "Logging")
    , _mutex()
    , _cond(_mutex)
    , _currentBuffer(new Buffer)
    , _nextBuffer(new Buffer)
    , _buffers()
    , _latch(1)
{
    assert(logFileName.size() > 1);
    _currentBuffer->bzero();
    _nextBuffer->bzero();
    _buffers.reserve(16);
}

void AsyncLogging::append(const char* logline, int len){
    MutexLockGuard lock(_mutex);
    if(_currentBuffer->avail() > len){
        _currentBuffer->append(logline, len);
    }else{
        _buffers.emplace_back(_currentBuffer);
        _currentBuffer.reset();
        if(_nextBuffer){
            _currentBuffer = std::move(_nextBuffer);
        }else{
            _currentBuffer.reset(new Buffer);
        }
        _currentBuffer->append(logline, len);
        _cond.notify();
    }
}
void AsyncLogging::threadFunc(){
    assert(_running == true);
    _latch.countDown();

    LogFile output(_basename);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    BufferVector buffersWrite;
    buffersWrite.reserve(16);
    while (_running){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersWrite.empty());
        
        {
            MutexLockGuard lock(_mutex);
            if(_buffers.empty()){
                _cond.waitForSeconds(_flushInterval);
            }
            _buffers.emplace_back(_currentBuffer);
            _currentBuffer.reset();

            _currentBuffer = std::move(newBuffer1);
            buffersWrite.swap(_buffers);
            if(!_nextBuffer){
                _nextBuffer = std::move(newBuffer2);
            }
        }
        assert(!buffersWrite.empty());

        if(buffersWrite.size() > 25){
            buffersWrite.erase(buffersWrite.begin() + 2, buffersWrite.end());
        }
        for(size_t i = 0; i<buffersWrite.size(); ++i){
            output.append(buffersWrite[i]->data(), buffersWrite[i]->length());
        }
        if(buffersWrite.size() > 2){
            buffersWrite.resize(2);
        }
        if(!newBuffer1){
            assert(!buffersWrite.empty());
            newBuffer1 = buffersWrite.back();
            buffersWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2){
            assert(!buffersWrite.empty());
            newBuffer2 = buffersWrite.back();
            buffersWrite.pop_back();
            newBuffer2->reset();
        }
        buffersWrite.clear();
        output.flush();
    }
    output.flush();
    
}
}