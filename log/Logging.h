#pragma once
#include"LogStream.h"

#include<stdio.h>
#include<string>
#include<string.h>
#include<pthread.h>

namespace ccy{

class AsyncLogging;

class Logger{
public:
    Logger(const char* filename, int line);
    ~Logger();
    LogStream &stream(){ return _impl._stream;}
    static void setLogFileName(const std::string fileName) {_logFileName = fileName;}
    static std::string getLogFileName() {return _logFileName;}

private:
class Impl{
public:
    Impl(const char* filename, int line);
    void formatTime();

    LogStream _stream;
    int _line;
    std::string _basename;
};
Impl _impl;
static std::string _logFileName;

};

#define LOG Logger(__FILE__, __LINE__).stream()
}