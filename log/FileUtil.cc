#include"FileUtil.h"

#include<assert.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>

namespace ccy{

AppendFile::AppendFile(std::string filename)
    : _fp(fopen(filename.c_str(), "ae"))
{   
    setbuffer(_fp, _buffer, sizeof _buffer);
}

AppendFile::~AppendFile(){fclose(_fp);}

void AppendFile::append(const char* logline, const size_t len){
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0){
        size_t x = this->write(logline + n, remain);
        if(x == 0){
            int err = ferror(_fp);
            if(err) fprintf(stderr, "AppendFile::append() failed!\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush(){fflush(_fp);}

// write len of logline into _fp
size_t AppendFile::write(const char* logline, size_t len){
    return fwrite_unlocked(logline, 1, len, _fp);
}

}