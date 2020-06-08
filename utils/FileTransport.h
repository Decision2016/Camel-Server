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
#include <algorithm>
#include <sys/stat.h>

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
    unsigned char key[32], token[32], iv[16];
    unsigned char recv_buffer[4096], send_buffer[4096], buffer[4096];
    int port, file_socket;
    long long lastTimestamp;
    Logger *logger;
    bool threadStatus, closeStatus;
    inline bool checkTimeout();

    void recvFile(std::string &_destination);
    void sendFile(std::string &_origin, unsigned long long _current);
    void aesEncrypto(unsigned char *in, unsigned char *out, int len);
    void aesDecrypto(unsigned char *in, unsigned char *out, int len);

    static unsigned long long getFileSize(std::string &_path);
    static void pushValue(unsigned char *destination, unsigned long long value, int bytes_len);
    static void popValue(unsigned char *from, unsigned long long &value, int bytes_len);
};


#endif //CAMEL_SERVER_FILETRANSPORT_H
