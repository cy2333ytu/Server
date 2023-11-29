#pragma once
#include"../utils/noncopyable.h"
#include<string>


namespace ccy{

class AppendFile: noncopyable{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();

    void append(const char*logline, const size_t len);
    void flush();
    
private:
    size_t write(const char* logline, size_t len);
    FILE *_fp;
    char _buffer[64 * 1024];
};

}