cmake_minimum_required(VERSION 3.15)
project(Camel_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Camel_Server main.cpp)

include_directories(logger
        utils
        threads)

add_library(common SHARED
        utils/FileManager.cpp
        logger/Logger.cpp
        utils/BaseClass.cpp
        threads/Session.cpp
        threads/Transporter.cpp
        camel_server.cpp)

target_link_libraries(Camel_Server common -lpthread -lssl -lcrypto -lini)