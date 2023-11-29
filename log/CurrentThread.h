#pragma once
#include<stdint.h>
#include<vector>

namespace ccy{
namespace CurrentThread{
/*
    The __thread keyword specifies that this variable is thread-local.
    Each thread has its own copy of this variable, 
    and changes to it in one thread do not affect its value in other threads
*/
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;


void cachedTid();

inline int tid(){
    if(__builtin_expect(t_cachedTid == 0, 0)){
        cachedTid();
    }
    return t_cachedTid;
}

inline const char* tidString(){
    return t_tidString;
}

inline int tidStringLength(){
    return t_tidStringLength;
}

inline const char* name(){return t_threadName;}

}

}