cmake_minimum_required(VERSION 3.15)
project(Camel_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Camel_Server main.cpp logger/Logger.cpp logger/Logger.h crypto/SHA_512.cpp crypto/SHA_512.h crypto/RSA.cpp crypto/RSA.h camel_server.cpp camel_server.h)