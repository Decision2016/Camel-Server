//
// Created by Decision on 2020/6/9.
//

#ifndef CAMEL_SERVER_SESSION_H
#define CAMEL_SERVER_SESSION_H

#include <openssl/rsa.h>
#include <string>
#include <thread>

#include "BaseClass.h"
#include "constants.h"
#include "Transporter.h"
#include "FileManager.h"


class Session : public BaseClass{
public:
    Session(int port, RSA *_rsa, Logger *_logger);
    ~Session();
    void threadInstance();
    void setUserInfo(char *_username, char *_password);
    void setWorkPath(const char* _path);
private:
    unsigned char recv_buffer[4096], send_buffer[4096], buffer[4096];
    char username[USERNAME_LENGTH], password[PASSWORD_LENGTH], path[PATH_LENGTH];
    int dirLevel = 0;
    RSA *keyPair;
    Transporter *tp = nullptr;
    std::string nowPath;

    void fileManage();
    int startFileThread();
    bool authUser(const unsigned char* buffer);
    void generateToken(unsigned char* out);
    void sendDirInfo();
};


#endif //CAMEL_SERVER_SESSION_H
