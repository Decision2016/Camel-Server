#include <iostream>
#include <unistd.h>
#include <inilib.h>
#include <string>

#include "constants.h"
#include "Logger.h"
#include "camel_server.h"

const int PATH_SIZE = 255;

std::string _username, _password, _workPath;
int _port, _low, _high;

bool config(Logger *logger);

int main(int argc, char **argv) {
    char workPath[PATH_SIZE];
    char* dirPath;
    Logger *logger = new Logger(workPath);

    if (!getcwd(workPath, PATH_SIZE)) {
        Logger().info("An error occurred while getting the path.", false);
        return 0;
    }
    logger->info("Camel is starting...");

    if (!config(logger)) {
        logger->info("Camel closed.");
        delete logger;
        return 0;
    }

    camel_server cs(_username, _password, logger, _port);
    cs.setPortLimit(_low, _high);
    if (cs.setWorkPath(_workPath) && cs.trySocket() != -1) {
        cs.serverInstance();
    }
    // todo: 进行网络环境检查
    logger->info("Camel closed.");
    delete logger;
    return 0;
}

bool config(Logger *logger) {
    INI::registry info("config.ini");

    _username = std::string(info["user"]["username"]);
    if (_username.length() <= 0 || _username.length() > USERNAME_LENGTH) {
        logger -> error("Username is too long or too short, it must between 0 and %d.", USERNAME_LENGTH);
        return false;
    }

     _password = std::string(info["user"]["password"]);
    if (_password.length() <= 0 || _password.length() > USERNAME_LENGTH) {
        logger -> error("Password is too long ot too short, it must between 0 and %d.", PASSWORD_LENGTH);
        return false;
    }

    _port = info["port"]["default"];
    if (_port == 0) {
        logger -> error("Must set listening port.");
        return false;
    }
    _low = info["port"]["low"];
    _high = info["port"]["high"];

    if (_low == 0 || _high == 0) {
        logger -> error("Must set port limit.");
        return false;
    }

    _workPath = std::string(info["path"]["path"]);

    return true;
}