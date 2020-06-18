//
// Created by Decision on 2020/6/9.
//

#ifndef CAMEL_SERVER_CAMEL_SERVER_H
#define CAMEL_SERVER_CAMEL_SERVER_H

#include <openssl/rsa.h>
#include <arpa/inet.h>
#include <string>
#include <inilib.h>

#include "BaseClass.h"
#include "constants.h"
#include "Session.h"

class camel_server: public BaseClass {
public:
    camel_server(std::string _username, std::string _password, Logger *_logger, int _port);
    void serverInstance();
    bool setWorkPath(std::string _path);
private:
    unsigned char recv_buffer[BUFFER_LENGTH], send_buffer[BUFFER_LENGTH], buffer[BUFFER_LENGTH];
    std::string path = "./camel";
    std::string username, password;
};


#endif //CAMEL_SERVER_CAMEL_SERVER_H
