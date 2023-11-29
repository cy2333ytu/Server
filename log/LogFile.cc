#include"LogFile.h"
#include"FileUtil.h"

#include<assert.h>
#include<stdio.h>
#include<time.h>

namespace ccy{

LogFile::LogFile(const std::string& basename, int flushEveryN)
    : _basename(basename)
    , _flushEveryN(flushEveryN)
    , _count(0)
    , _mutex(new MutexLock){
        _file.reset(new AppendFile(basename));
    }

LogFile::~LogFile(){}

void LogFile::append(const char* logline, int len){
    MutexLockGuard lock(*_mutex);
    append_unlocked(logline, len);
}
void LogFile::flush(){
    MutexLockGuard lock(*_mutex);
    _file->flush();
}

void LogFile::append_unlocked(const char* logline, int len){
    _file->append(logline, len);
    ++_count;
    if(_count >= _flushEveryN){
        _count = 0;
        _file->flush();
    }
}



}
