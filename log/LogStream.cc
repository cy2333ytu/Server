#include"LogStream.h"

#include<assert.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<limits>

namespace ccy
{
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// 把数字转化为 char 并存储在 buf 中
// 返回值是转化后的长度
template<typename T>
size_t convert(char buf[], T value){
    T i = value;
    char* p = buf;

    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    }while(i != 0);

    if(value < 0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

template<typename T>
void LogStream::formatInteger(T c){
    // 把buffer_ 长度增加 v 
    if(buffer_.avail() >= kMaxNumberSize){
        size_t len = convert(buffer_.current(), c);
        buffer_.add(len);
    }
}
/*
    Promotion to int: In many C++ implementations, when pass a short 
    to a variadic function or use it in certain expressions, it gets 
    automatically promoted to an int. 
    Performing the explicit conversion ensures consistent behavior 
    and avoids potential issues related to automatic promotion.

    Consistency in Function Overloads: the LogStream class has other 
    operator<< overloads that accept int, converting short to int ensures 
    that the same processing logic is applied to all integer-like types. 
    This can lead to more consistent and maintainable code.
*/
LogStream& LogStream::operator<<(short c){
    *this << static_cast<int>(c);
    return *this;
}
LogStream& LogStream::operator<<(unsigned short c){
    *this << static_cast<unsigned int>(c);
    return *this;
}
LogStream& LogStream::operator<<(int c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(unsigned int c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(long c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(long long c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long long c){
    formatInteger(c);
    return *this;
}
LogStream& LogStream::operator<<(double c){
    if(buffer_.avail() >= kMaxNumberSize){
        int len = snprintf(buffer_.current(), kMaxNumberSize, "%.12g", c);
        buffer_.add(len);
    }
    return *this;
}
LogStream& LogStream::operator<<(long double c){
  if (buffer_.avail() >= kMaxNumberSize) {
    int len = snprintf(buffer_.current(), kMaxNumberSize, "%.12Lg", c);
    buffer_.add(len);
  }
  return *this;
}

}