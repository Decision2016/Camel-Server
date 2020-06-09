#include <iostream>
#include <unistd.h>

#include "Logger.h"
#include "camel_server.h"

const int PATH_SIZE = 255;

char username[] = "Decision";
char password[] = "123456";


int main(int argc, char **argv) {
    char workPath[PATH_SIZE];
    char* dirPath;
    Logger *logger = new Logger(workPath);

    if (!getcwd(workPath, PATH_SIZE)) {
        Logger().info("An error occurred while getting the path.", false);
        return 0;
    }
    logger->info("Camel is starting...");

    camel_server cs(username, password, logger, 25565);
    if (cs.trySocket()) {
        cs.serverInstance();
    }
    // todo: 进行网络环境检查
    logger->info("Camel closed.");
    delete logger;
    return 0;
}