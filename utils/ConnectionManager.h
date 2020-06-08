//
// Created by Decision on 2020/5/12.
//

#ifndef CAMEL_SERVER_CONNECTIONMANAGER_H
#define CAMEL_SERVER_CONNECTIONMANAGER_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <netinet/in.h>
#include <mutex>
#include <thread>
#include <string>
#include <algorithm>

#include "FileManager.h"
#include "../logger/Logger.h"
#include <openssl/err.h>
#include "FileTransport.h"
#include "../constants.h"
static const int MAX_TIME_WAITING = 300;

class ConnectionManager {
public:
    ConnectionManager(int _port, RSA *_rsa, Logger *_logger);
    ~ConnectionManager();
    void setUserInfo(char *_username, char *_password);
    void startConnection();
    void setWorkPath(const char* _path);
private:
    char username[16], password[16], path[32];
    unsigned char token[32], key[32], iv[16];
    int fileConnectFd;
    int port, filePort;
    int dirLevel = 0;
    long long lastTimestamp;
    RSA *connectRSA;
    AES_KEY aesKey;
    Logger *logger;
    bool thread_status = false;
    FileTransport *ft = nullptr;
    std::string nowPath;

    void startFileThread();
    bool authUser(const unsigned char *buffer);
    void fileManage(const int &connect_fd);
    void generateToken();
    bool checkToken(unsigned char buffer[32]);
    inline bool checkTimeout(long long timeLimit = MAX_TIME_WAITING);
    inline void clearIv();
    void aesEncrypt(const unsigned char* in, unsigned char* out, int len);
    void aesDecrypt(const unsigned char* in, unsigned char* out, int len);
    void sendStatusCode(int statusCode, const int& connect_fd);

    void sendDirInfo(const int &connect_fd);
    void enterDir(const int &connect_fd);

    static void getStatusCode(int &statusCode, const unsigned char* buffer);
    static void putStatusCode(const int &statusCode, unsigned char &firstChar, unsigned char &secondChar);
    static void sha256(unsigned char* buffer,unsigned char *out, int length);
    static void getValue(unsigned char *from, unsigned long long &value, int bytes_len);
    static void pushValue(unsigned char *destination, unsigned long long value, int bytes_len);
    static int chosePort();
    static bool checkPort(int port);
};


#endif //CAMEL_SERVER_CONNECTIONMANAGER_H