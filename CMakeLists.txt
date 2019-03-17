cmake_minimum_required(VERSION 3.13)
project(KP)

set(CMAKE_CXX_STANDARD 11)

include_directories(code)

add_executable(KP
        data.h
        server.cpp
        #/client.cpp
)

target_link_libraries(KP zmq)