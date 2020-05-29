//
// Created by Decision on 2020/5/12.
//


#include "ConnectionManager.h"
#include "../camel_server.h"


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
            if (checkTimeout()) {
                logger -> info("Connect timeout on port %d, close socket thread.", port);
                break;
            }
            else continue;
        }
        n = recv(connectfd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        if (statusCode == SECOND_CONNECT) {
            if (authUser(&buffer[2])) {
                logger -> success("User authorized successful, start file transport on port %d.", port);
                fileManage(listenfd);
            }
            else {
                logger->warning("User authorized failed.");
            }
        }
        else {
            logger -> warning("Received a connect request, but status code is error.");
        }
        lastTimestamp = time(nullptr);
    }
    close(listenfd);
}

void ConnectionManager::setUserInfo(char *_username, char *_password) {
    strcpy(username, _username);
    strcpy(password, _password);
}

bool ConnectionManager::authUser(const char *buffer) {
    char _username[16], _password[16];
    unsigned char plainText[4096];
    int length = strlen(buffer);
    int result = RSA_private_decrypt(length, (unsigned char*) buffer, plainText, connectRSA, RSA_PKCS1_PADDING);
    memcpy(_username, plainText, 16);
    memcpy(_password, &plainText[16], 16);
    if (strcmp(username, _username) != 0 || strcmp(password, _password) != 0) return false;
    AES_set_encrypt_key(&plainText[1296], 256, &aesKey);
    BIO *keybio = BIO_new_mem_buf((unsigned char*) &plainText[32], 1024);
    userKey = RSA_new();
    userKey = PEM_read_bio_RSA_PUBKEY(keybio, &userKey, nullptr, nullptr);
    BIO_free_all(keybio);
    // todo: 报错处理
    return true;
}

void ConnectionManager::fileManage(const int &listen_fd) {
    int connect_fd;
    int statusCode, length;
    char buffer[4096];
    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*) nullptr, nullptr)) == -1) {
            if (checkTimeout()) break;
            else continue;
        }
        length = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        switch (statusCode) {
            case REFRESH_DIR:

                break;
            case GOTO_DIR:

                break;
            case BACK_DIR:

                break;
            case DELETE_DIR:

                break;
            case CREATE_DIR:

                break;
            case CLOSE_CONNECT:

                break;
            case POST_FILE:

                break;
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