//
// Created by Decision on 2020/5/8.
//

#ifndef CAMEL_SERVER_CAMEL_SERVER_H
#define CAMEL_SERVER_CAMEL_SERVER_H

#include <cstdlib>
#include <random>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include "./logger/Logger.h"
#include "utils/ConnectionManager.h"

static const int FIRST_CONNECT = 210;

class camel_server {
public:
    camel_server(char* _username, char* _password, Logger *_logger);
    void startServer();
    void setPort(int _port);
    void setWorkPath(char *_path);
    void setMaxThread(int _maxThread);

private:
    char username[16], password[16];
    int port;
    char *path;
    int maxThread;
    Logger *logger;

    static void getStatusCode(int &statusCode, const char* buffer);
    static void putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
    static int chosePort();
    static bool checkPort(int port);
};


#endif //CAMEL_SERVER_CAMEL_SERVER_H
