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

    while(true) {
        if(threadStatus == true) {
            logger -> info("file transport thread stopping.");
            break;
        }

        if ((connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr)) == -1) {
            if (checkTimeout()) {
                logger -> info("File transport timeout on port %d, close file transport socket thread.", port);
                break;
            }
            else continue;
        }
        n = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        switch (statusCode) {
            case POST_FILE: {
                break;
            }
            case FILE_NOT_END: {
                break;
            }
            case FILE_END: {
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