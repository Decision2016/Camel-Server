//
// Created by Decision on 2020/5/24.
//

#include "FileTransport.h"

FileTransport::FileTransport(AES_KEY _aesKey, int _port): aesKey(_aesKey), port(_port) {}

void FileTransport::startThread() {
    int listen_fd, connect_fd, n, statuCode;
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
        if ((connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr)) == -1) {
            if (checkTimeout()) {
                logger -> info("Connect timeout on port %d, close file transport socket thread.", port);
                break;
            }
            else continue;
        }
        n = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statuCode, buffer);
        switch (statuCode) {
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
    }
}

bool FileTransport::checkTimeout() {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= MAX_TIME_WAITING;
}

void FileTransport::getStatusCode(int &statusCode, const char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void FileTransport::putStatusCode(const int &statusCode, char &firstChar, char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}