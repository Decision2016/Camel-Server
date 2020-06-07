//
// Created by Decision on 2020/5/24.
//

#include "FileTransport.h"

FileTransport::FileTransport(const unsigned char* _key, int _port, Logger *_logger): port(_port), logger(_logger) {
    threadStatus = false;
    memcpy(key, _key, 32);
}

void FileTransport::startThread() {
    int listen_fd, connect_fd, n, statusCode;
    char buffer[4096];
    sockaddr_in socketServerStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while start file transport thread.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = port;
    socketServerStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port in file transport thread.");
        return ;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket in file transport thread.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d in file transport thread.", port);
    connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr);

    if (connect_fd != -1) {
        logger -> info ("Connect with client successful.");
    }

    while(true) {
        if(threadStatus == true) {
            logger -> info("file transport thread stopping.");
            break;
        }
        n = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        switch (statusCode) {
            case FILE_UPLOAD: {
                getStatusCode(statusCode, buffer);
                break;
            }
        }
        close(connect_fd);
    }
    close(listen_fd);
    closeStatus = false;
}

bool FileTransport::checkTimeout() {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= 600;
}

void FileTransport::getStatusCode(int &statusCode, const char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void FileTransport::putStatusCode(const int &statusCode, char &firstChar, char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}

void FileTransport::stopThread() {
    threadStatus = false;
}

void FileTransport::setToken(unsigned char *buffer) {
    memcpy(token, buffer, 32);
}

bool FileTransport::getCloseStatus() {
    return closeStatus;
}