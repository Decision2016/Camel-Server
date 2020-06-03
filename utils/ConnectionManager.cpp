//
// Created by Decision on 2020/5/12.
//


#include "ConnectionManager.h"
#include "../camel_server.h"


ConnectionManager::ConnectionManager(int _port, RSA *_rsa, Logger *_logger) :port(_port), connectRSA(_rsa), logger(_logger){}

void ConnectionManager::startConnection() {
    int listen_fd, connect_fd, n, statusCode;
    unsigned char recv_buffer[4096], send_buffer[4096];
    sockaddr_in socketServerStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while create server.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = htons(port);
    socketServerStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port.");
        return ;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d.", port);

    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr)) == -1) {
            if (checkTimeout()) {
                logger -> info("Connect timeout on port %d, close socket thread.", port);
                break;
            }
            else continue;
        }
        n = recv(connect_fd, recv_buffer, 4096, 0);
        getStatusCode(statusCode, recv_buffer);
        if (statusCode == SECOND_CONNECT) {
            logger->info("Received authorized request, start auth user.");
            if (authUser(&recv_buffer[2])) {
                logger -> success("User authorized successful, start file transport on port %d.", port);
                fileManage(listen_fd);
            }
            else {
                logger->warning("User authorized failed.");
            }
        }
        else {
            logger -> warning("Received a connect request, but status code %d is error.", statusCode);
        }
        lastTimestamp = time(nullptr);
    }
    close(listen_fd);
}

void ConnectionManager::setUserInfo(char *_username, char *_password) {
    strcpy(username, _username);
    strcpy(password, _password);
}

bool ConnectionManager::authUser(const unsigned char *buffer) {
    char _username[16], _password[16];
    unsigned char plainText[4096];
    int result = RSA_private_decrypt(256, (unsigned char*) buffer, plainText, connectRSA, RSA_PKCS1_PADDING);
    if (result < 0) {
        ERR_print_errors_fp(stdout);
    }
    memcpy(_username, plainText, 16);
    memcpy(_password, &plainText[16], 16);
    if (strcmp(username, _username) != 0 || strcmp(password, _password) != 0) return false;
    AES_set_encrypt_key(&plainText[32], 256, &aesKey);
    // todo: 报错处理
    return true;
}

void ConnectionManager::fileManage(const int &listen_fd) {
    int connect_fd;
    int statusCode, length;
    unsigned char recv_buffer[4096], send_buffer[4096];
    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*) nullptr, nullptr)) == -1) {
            if (checkTimeout()) break;
            else continue;
        }
        length = recv(connect_fd, recv_buffer, 4096, 0);
        getStatusCode(statusCode, recv_buffer);
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
            case REQUIRE_PORT: {
                // todo: choose port
                if (!thread_status) {
                    FileTransport ft(aesKey, filePort, logger);
                    std::thread childThread(std::bind(&FileTransport::startThread, &ft, thread_status));
                    childThread.detach();
                }
                memset(recv_buffer, 0, sizeof(recv_buffer));
                putStatusCode(150, recv_buffer[0], recv_buffer[1]);
                recv_buffer[2] = char(filePort);
                send(connect_fd, recv_buffer, 4096, 0);
                break;
            }
        }
        close(connect_fd);
    }
    close(listen_fd);
}

bool ConnectionManager::checkTimeout() {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= MAX_TIME_WAITING;
}

void ConnectionManager::getStatusCode(int &statusCode, const unsigned char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void ConnectionManager::putStatusCode(const int &statusCode, unsigned char &firstChar, unsigned char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}