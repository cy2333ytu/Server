#include"Logging.h"
#include"AsyncLogging.h"
#include"../utils/Thread.h"
#include"CurrentThread.h"

#include<assert.h>
#include<iostream>
#include<time.h>
#include<sys/time.h>
#include<memory>

namespace ccy{

static pthread_once_t onceControl = PTHREAD_ONCE_INIT;
static AsyncLogging *_AsyncLogger;
std::string Logger::_logFileName = "./testFile.log";

void once_init(){
    _AsyncLogger = new AsyncLogging(Logger::getLogFileName());
    _AsyncLogger->start();
}
void output(const char* msg, int len){
    pthread_once(&onceControl, once_init);
    _AsyncLogger->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line)
  : _stream(),
    _line(line),
    _basename(fileName)
{
    formatTime();
}
void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    _stream << str_t;
}

Logger::Logger(const char *fileName, int line)
  : _impl(fileName, line)
{ }

Logger::~Logger()
{
    _impl._stream << " -- " << _impl._basename << ':' << _impl._line << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}
}