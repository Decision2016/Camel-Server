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

#include "../constants.h"
#include "../logger/Logger.h"

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
    int port, file_socket;
    long long lastTimestamp;
    Logger *logger;
    bool threadStatus, closeStatus;
    inline bool checkTimeout();

    void downloadFile(std::string _destination);
    void uploadFile(std::string _origin);

    void static getStatusCode(int &statusCode, const char *buffer);
    void static putStatusCode(const int &statusCode, char &firstChar, char &secondChar);
};


#endif //CAMEL_SERVER_FILETRANSPORT_H
