cmake_minimum_required(VERSION 3.15)
project(Camel_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Camel_Server main.cpp logger/Logger.cpp logger/Logger.h camel_server.cpp camel_server.h utils/FileManager.cpp utils/FileManager.h utils/ConnectionManager.cpp utils/ConnectionManager.h)