//
// Created by Decision on 2020/5/12.
//

#ifndef CAMEL_SERVER_CONNECTIONMANAGER_H
#define CAMEL_SERVER_CONNECTIONMANAGER_H

#include <openssl/rsa.h>
#include <sys/socket.h>
#include <cstring>
#include <ctime>

#include "../logger/Logger.h"

const int MAX_TIME_WAITING = 300;

class ConnectionManager {
public:
    ConnectionManager(int _port, RSA *_rsa, Logger *_logger);
    void startConnection();
private:
    int port;
    long long lastTimestamp;
    RSA *connectRSA;
    Logger *logger;

    bool authUser(const int &listen_fd);
    bool fileManage(const int &listen_fd);
    inline bool checkTimeout();
    static void getStatusCode(int &statusCode, const char* buffer);
    static void putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
};


#endif //CAMEL_SERVER_CONNECTIONMANAGER_H
