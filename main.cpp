#include <iostream>

#include <unistd.h>

#include "logger/Logger.h"

const int PATH_SIZE = 255;

int main(int argc, char **argv) {
    char workPath[PATH_SIZE];
    char* dirPath;

    if (!getcwd(workPath, PATH_SIZE)) {
        Logger().info("An error occurred while getting the path.", false);
        return 0;
    }
    Logger(workPath).info("Camel is starting...");

    // todo: 进行网络环境检查


    return 0;
}