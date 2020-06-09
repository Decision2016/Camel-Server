//
// Created by Decision on 2020/6/9.
//

#ifndef CAMEL_SERVER_TRANSPORTER_H
#define CAMEL_SERVER_TRANSPORTER_H

#include <sys/stat.h>

#include "BaseClass.h"
#include "Logger.h"

class Transporter: public BaseClass {
public:
    Transporter(int _port, Logger *_logger);
    void threadInstance();
private:
    unsigned char recv_buffer[4096], send_buffer[4096], buffer[4096];

    void recvFile(std::string &_destination, unsigned long long _current);
    void sendFile(std::string &_origin, unsigned long long _current);
    static unsigned long long getFileSize(std::string &_path);
};


#endif //CAMEL_SERVER_TRANSPORTER_H
