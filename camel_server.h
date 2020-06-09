//
// Created by Decision on 2020/6/9.
//

#ifndef CAMEL_SERVER_CAMEL_SERVER_H
#define CAMEL_SERVER_CAMEL_SERVER_H

#include <openssl/rsa.h>
#include <arpa/inet.h>

#include "BaseClass.h"
#include "constants.h"
#include "Session.h"

class camel_server: public BaseClass {
public:
    camel_server(char* _username, char* _password, Logger *_logger, int _port);
    void serverInstance();
    void setWorkPath(char *_path);
private:
    unsigned char recv_buffer[BUFFER_LENGTH], send_buffer[BUFFER_LENGTH], buffer[BUFFER_LENGTH];
    void createWorkPath();
    char path[PATH_LENGTH] = "camel";
    char username[USERNAME_LENGTH], password[PASSWORD_LENGTH];
};


#endif //CAMEL_SERVER_CAMEL_SERVER_H
