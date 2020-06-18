//
// Created by Decision on 2020/6/9.
//

#ifndef CAMEL_SERVER_BASECLASS_H
#define CAMEL_SERVER_BASECLASS_H

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "Logger.h"
#include "constants.h"

class BaseClass {
public:
    BaseClass(int _port, Logger* _logger);

    bool trySocket();
    void setConnect(int _connect_fd);
    void setKey(unsigned char* _key);
    void setToken(unsigned char* _token);
    void setPort(int _port);
    void setPortLimit(int _low, int _high);
    void aesEncrypt(unsigned char *in, unsigned char *out, int len);
    void aesDecrypt(unsigned char *in, unsigned char *out, int len);
    void sendStatusCode(unsigned long long _statusCode);
    void stopThread();
    bool checkToken(unsigned char buffer[32]);
    bool getThreadStatus();

    static void pushValue(unsigned char *destination, unsigned long long value, int bytes_len);
    static void popValue(unsigned char *origin, unsigned long long &value, int bytes_len);
    static void sha256(unsigned char* buffer,unsigned char *out, int length);
    int choosePort();
    bool checkPort(int port);
private:
    AES_KEY aesKey;
    bool threadStatus = false;
    unsigned char  iv[IV_LENGTH];
    unsigned char _buffer[BUFFER_LENGTH];

protected:
    unsigned char key[KEY_LENGTH], token[TOKEN_LENGTH];
    int lowerPort, higherPort;
    int connect_fd, listen_fd;
    int port;
    Logger *logger;
};


#endif //CAMEL_SERVER_BASECLASS_H
