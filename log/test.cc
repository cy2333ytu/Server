#include"./Logging.h"
#include"../utils/Thread.h"

#include<string>
#include<unistd.h>
#include<iostream>
#include<memory>
#include<vector>

void threadFunc(){
    for(int i = 0; i< 100000; ++i){
        ccy::LOG << i;
    }
}

void testType(){
    std::cout<< "------------------testType-----------------" <<std::endl;
    ccy::LOG << 11;
    ccy::LOG << 99999999999999999;
    ccy::LOG << 4.36f;
    ccy::LOG << 1.0;
    ccy::LOG << static_cast<short> (6);
    ccy::LOG << static_cast<long long> (6);
    ccy::LOG << static_cast<unsigned int> (6);
    ccy::LOG << static_cast<unsigned long> (6);
    ccy::LOG << static_cast<long double> (6.66666666);
    ccy::LOG << static_cast<unsigned long long> (6);
    ccy::LOG << 'c';
    ccy::LOG << "qwertyu";
    ccy::LOG << std::string("qwertyu");

}

void stressing_single_thread()
{
    // 100000 lines
    std::cout << "----------stressing test single thread-----------" << std::endl;
    for (int i = 0; i < 100000; ++i)
    {
        ccy::LOG << i;
    }
}

void stressing_multi_threads(int threadNum = 4)
{
    // threadNum * 100000 lines
    std::cout << "----------stressing test multi thread-----------" << std::endl;
    std::vector<std::shared_ptr<ccy::Thread>> vsp;
    for (int i = 0; i < threadNum; ++i)
    {
        std::shared_ptr<ccy::Thread> tmp(new ccy::Thread(threadFunc, "testFunc"));
        vsp.emplace_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->start();
    }
    sleep(3);
}

void other()
{
    // 1 line
    std::cout << "----------other test-----------" << std::endl;
    ccy::LOG << "fddsa" << 'c' << 0 << 3.666 << std::string("This is a string");
}


int main(){
    ccy::Logger::setLogFileName("../logFile/logTest.log");
    testType();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    stressing_multi_threads();
    sleep(3);

    other();
    sleep(3);
    return 0;
}


