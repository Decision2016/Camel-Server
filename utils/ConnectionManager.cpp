//
// Created by Decision on 2020/5/12.
//

#include <netinet/in.h>
#include "ConnectionManager.h"


ConnectionManager::ConnectionManager(int _port, RSA *_rsa, Logger *_logger) :port(_port), connectRSA(_rsa), logger(_logger){}

void ConnectionManager::startConnection() {
    int listenfd, connectfd, n, statusCode;
    char buffer[4096];
    sockaddr_in socketServerStruct;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while create server.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = port;
    socketServerStruct.sin_family = AF_INET;

    if (bind(listenfd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port.");
        return ;
    }

    if (listen(listenfd, 10) == -1) {
        logger -> error("An error occurred while listen socket.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d", port);

    while (true) {
        if ((connectfd = accept(listenfd, (sockaddr*)nullptr, nullptr)) == -1) {
            if (checkTimeout()) break;
            else continue;
        }
        n = recv(connectfd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        if (statusCode == 110) {
            if (authUser(listenfd)) {
                fileManage(listenfd);
            }
            else {
                // todo: return refuse information

            }
        }
        lastTimestamp = time(nullptr);
    }

}

bool ConnectionManager::authUser(const int &listen_fd) {
    int connect_fd;
    int n, statusCode;
    char buffer[4096];
    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*) nullptr, nullptr)) == -1) {
            if (checkTimeout()) break;
            else continue;
        }
        n = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        if (statusCode == 211) {
        }
        else {

        }
        lastTimestamp = time(nullptr);
    }
}

bool ConnectionManager::fileManage(const int &listen_fd) {
    int connect_fd;
    int statusCode, length;
    char buffer[4096];
    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*) nullptr, nullptr)) == -1) {
            if (checkTimeout()) break;
            else continue;
        }
    }
}

bool ConnectionManager::checkTimeout() {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= MAX_TIME_WAITING;
}

void ConnectionManager::getStatusCode(int &statusCode, const char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void ConnectionManager::putStatusCode(const int &statusCode, char &firstChar, char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}