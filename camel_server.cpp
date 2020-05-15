//
// Created by Decision on 2020/5/8.
//

#include "camel_server.h"

camel_server::camel_server(char *_username, char *_password, Logger *_logger) : logger(_logger) {
        strcpy(username, _username);
        strcpy(password, _password);
    }

void camel_server::startServer() {
    int listen_fd, connect_fd, childListen_fd;
    int length, statusCode;
    char buffer[4096];
    sockaddr_in socketServerStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while create server.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = port;
    socketServerStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port.");
        return ;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d", port);

    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr)) == -1) {
            continue;
        }
        length = recv(connect_fd, buffer, 4096, 0);
        getStatusCode(statusCode, buffer);
        if (statusCode == FIRST_CONNECT) {
            int randPort = chosePort();
            RSA* keyPair = RSA_new();
            int res = 0;
            BIGNUM* bignum = BN_new();
            res = BN_set_word(bignum, RSA_F4);
            res = RSA_generate_key_ex(keyPair, 1024, bignum, NULL);

            if (res != 1) {
                logger->error("Some error occurred while generate RSA key pair.");
                continue;
            }

            ConnectionManager cm(randPort,keyPair, logger);
            cm.setUserInfo(username, password);
            std::thread tranThread(&ConnectionManager::startConnection, cm);
            tranThread.detach();
            logger -> success("One file transport thread start running on port %d", randPort);
        }
        close(connect_fd);
    }
    close(listen_fd);
}

void camel_server::setPort(int _port) {
    port = _port;
}

void camel_server::setMaxThread(int _maxThread) {
    maxThread = _maxThread;
}

void camel_server::setWorkPath(char *_path) {
    path = _path;
}

void camel_server::getStatusCode(int &statusCode, const char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void camel_server::putStatusCode(const int &statusCode, char &firstChar, char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}

int camel_server::chosePort() {
    srand(time(nullptr));
    int randPort = 0;
    while (!checkPort(randPort)) {
        randPort = rand() % 65535;
    }
    return randPort;
}

bool camel_server::checkPort(int port) {
    if (port < 30000 || port > 65535) return false;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    memset(&sin, 0, sizeof(0));
    sin.sin_family = AF_INET;
    sin.sin_port = port;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socket_fd, (sockaddr*) &sin, sizeof(sockaddr)) < 0) {
        close(socket_fd);
        return false;
    }
    close(socket_fd);
    return true;
}