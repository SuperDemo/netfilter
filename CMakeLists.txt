cmake_minimum_required(VERSION 3.6)
project(netfilter)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    client/client.c
    conf/data.txt
    conf/file.txt
    conf/netfilter.conf
    src/alarmDetection.c
    src/alarmDetection.h
    src/communication.c
    src/communication.h
    src/dealConf.c
    src/dealConf.h
    src/log.h
    src/Makefile
    src/makes.sh
    src/netFilter.c
    src/netFilter.h
    threadTest/kthread.c
    threadTest/Makefile)

add_executable(netfilter ${SOURCE_FILES})