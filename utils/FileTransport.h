//
// Created by Decision on 2020/5/24.
//

#ifndef CAMEL_SERVER_FILETRANSPORT_H
#define CAMEL_SERVER_FILETRANSPORT_H

#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mutex>
#include <unistd.h>
#include <functional>

#include "../logger/Logger.h"

static const int POST_FILE = 220;
static const int FILE_NOT_END = 221;
static const int FILE_END = 222;

class FileTransport {
public:
    FileTransport(const unsigned char* _key, int _port, Logger *_logger);
    void stopThread();
    void startThread();
    bool getCloseStatus();
    void setToken(unsigned char* buffer);
private:
    AES_KEY aesKey;
    unsigned char key[32], token[32];
    int port;
    long long lastTimestamp;
    Logger *logger;
    bool threadStatus, closeStatus;
    inline bool checkTimeout();
    void static getStatusCode(int &statusCode, const char *buffer);
    void static putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
};


#endif //CAMEL_SERVER_FILETRANSPORT_H
