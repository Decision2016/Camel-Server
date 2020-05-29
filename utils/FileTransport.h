//
// Created by Decision on 2020/5/24.
//

#ifndef CAMEL_SERVER_FILETRANSPORT_H
#define CAMEL_SERVER_FILETRANSPORT_H

#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../logger/Logger.h"

static const int MAX_TIME_WAITING = 600;
static const int POST_FILE = 220;
static const int FILE_NOT_END = 221;
static const int FILE_END = 222;

class FileTransport {
public:
    void startThread();
    FileTransport(AES_KEY _aesKey, int _port);
private:
    AES_KEY aesKey;
    int port;
    long long lastTimestamp;
    Logger *logger;
    inline bool checkTimeout();
    void static getStatusCode(int &statusCode, const char *buffer);
    void static putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
};


#endif //CAMEL_SERVER_FILETRANSPORT_H