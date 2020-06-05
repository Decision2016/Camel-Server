cmake_minimum_required(VERSION 3.15)
project(Camel_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Camel_Server main.cpp)

add_library(common SHARED
        camel_server.cpp
        utils/FileManager.cpp
        utils/ConnectionManager.cpp
        utils/FileTransport.cpp
        logger/Logger.cpp utils/FileInfo.cpp utils/FileInfo.h)

target_link_libraries(Camel_Server common -lpthread -lssl -lcrypto)