cmake_minimum_required(VERSION 3.6)
project(tcpserver)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES udpTest/udpclient.c)

add_executable(client ${SOURCE_FILES})