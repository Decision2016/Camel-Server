#include <iostream>
#include <unistd.h>
#include <inilib.h>
#include <string>

#include "constants.h"
#include "Logger.h"
#include "camel_server.h"

const int PATH_SIZE = 255;

char username[USERNAME_LENGTH];
char password[PASSWORD_LENGTH];
std::string _workPath;
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

    camel_server cs(username, password, logger, _port);
    cs.setPortLimit(_low, _high);
    if (cs.setWorkPath(_workPath) && cs.trySocket()) {
        cs.serverInstance();
    }
    // todo: 进行网络环境检查
    logger->info("Camel closed.");
    delete logger;
    return 0;
}

bool config(Logger *logger) {
    std::string temporary;
    INI::registry info("config.ini");
    memset(username, 0, USERNAME_LENGTH);
    memset(password, 0, PASSWORD_LENGTH);

    temporary = std::string(info["user"]["username"]);
    if (temporary.length() <= 0 || temporary.length() > USERNAME_LENGTH) {
        logger -> error("Username is too long or too short, it must between 0 and %d.", USERNAME_LENGTH);
        return false;
    }
    memcpy(username, temporary.c_str(), temporary.length());

    temporary = std::string(info["user"]["password"]);
    if (temporary.length() <= 0 || temporary.length() > USERNAME_LENGTH) {
        logger -> error("Password is too long ot too short, it must between 0 and %d.", PASSWORD_LENGTH);
        return false;
    }
    memcpy(password, temporary.c_str(), temporary.length());

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