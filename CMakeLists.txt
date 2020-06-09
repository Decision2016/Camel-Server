cmake_minimum_required(VERSION 3.15)
project(Camel_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Camel_Server main.cpp utils/constants.h threads/Transporter.cpp threads/Transporter.h threads/Session.cpp threads/Session.h camel_server.cpp camel_server.h)

include_directories(logger
        utils
        threads)

add_library(common SHARED
        utils/FileManager.cpp
        logger/Logger.cpp utils/BaseClass.cpp utils/BaseClass.h utils/Configer.cpp utils/Configer.h)

target_link_libraries(Camel_Server common -lpthread -lssl -lcrypto)