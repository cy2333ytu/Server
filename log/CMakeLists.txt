cmake_minimum_required(VERSION 3.12)
project(log)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

set(SOURCES
    AsyncLogging.cc
    CountDownLatch.cc
    FileUtil.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    ../utils/Thread.cc
)

set(HEADERS
    AsyncLogging.h
    Condition.h
    CountDownLatch.h
    CurrentThread.h
    FileUtil.h
    LogFile.h
    Logging.h
    LogStream.h
    ../utils/MutexLock.h
    ../utils/noncopyable.h
    ../utils/Thread.h
)
add_executable(testLog test.cc ${SOURCES} ${HEADERS})

