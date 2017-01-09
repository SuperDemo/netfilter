cmake_minimum_required(VERSION 3.6)
project(tcpserver)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES client/netlink_client.c)

add_executable(client ${SOURCE_FILES})