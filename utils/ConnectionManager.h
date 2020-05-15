//
// Created by Decision on 2020/5/12.
//

#ifndef CAMEL_SERVER_CONNECTIONMANAGER_H
#define CAMEL_SERVER_CONNECTIONMANAGER_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <sys/socket.h>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <netinet/in.h>
#include "../logger/Logger.h"

static const int REFRESH_DIR = 200;
static const int GOTO_DIR = 201;
static const int BACK_DIR = 202;
static const int DELETE_DIR = 203;
static const int CREATE_DIR = 204;
static const int SECOND_CONNECT = 211;
static const int CLOSE_CONNECT = 212;
static const int POST_FILE = 220;
static const int FILE_NOT_END = 221;
static const int FILE_END = 222;
static const int RENAME_DIR = 223;
static const int DELETE_FILE = 224;
static const int COPY_FILE = 225;
static const int MOVE_FILE = 226;
static const int FILE_DISCONNECT = 227;

const int MAX_TIME_WAITING = 300;

class ConnectionManager {
public:
    ConnectionManager(int _port, RSA *_rsa, Logger *_logger);
    void setUserInfo(char *_username, char *_password);
    void startConnection();
private:
    char username[16], password[16];
    int port;
    long long lastTimestamp;
    RSA *connectRSA, *userKey;
    char aesKey[32];
    Logger *logger;

    bool authUser(const char *buffer);
    void fileManage(const int &listen_fd);
    void getFile(const int &listen_fd);
    inline bool checkTimeout();
    static void getStatusCode(int &statusCode, const char* buffer);
    static void putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
};


#endif //CAMEL_SERVER_CONNECTIONMANAGER_H
