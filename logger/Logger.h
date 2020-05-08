//
// Created by Decision on 2020/5/7.
//

#ifndef CAMEL_SERVER_LOGGER_H
#define CAMEL_SERVER_LOGGER_H

#include <cstring>
#include <cstdio>
#include <ctime>

enum Status {
    Success,
    Error,
    Info,
    Warning
};

class Logger {
public:
    void success(char *sMsg, bool saveToFile = true);
    void error(char *sMsg, bool saveToFile = true);
    void info(char *sMsg, bool saveToFile = true);
    void warning(char *sMsg, bool saveToFile = true);
    Logger(char *cLogPath);
    Logger();
    ~Logger();

private:
    const int dStringLength = 1000;

    char *logPath = nullptr;

    bool fnWriteFile(const char* output);
    void fnBaseLogFunction(enum Status status, char *sMsg, bool saveToFile);

    static const char* fnGetHeader(enum Status status);
    static char* fnGetTimeString();
    static void fnJointString(char* destination,const char *headString, char* timeString, char* msgString);
};


#endif //CAMEL_SERVER_LOGGER_H
