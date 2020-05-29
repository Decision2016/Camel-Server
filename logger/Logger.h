//
// Created by Decision on 2020/5/7.
//

#ifndef CAMEL_SERVER_LOGGER_H
#define CAMEL_SERVER_LOGGER_H

#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstdarg>

enum Status {
    Success,
    Error,
    Info,
    Warning
};

class Logger {
public:
    void success(char *__format, ...);
    void error(char *__format, ...);
    void info(char *__format, ...);
    void warning(char *__format, ...);
    Logger(char *cLogPath);
    Logger();
    ~Logger();

private:
    const int dStringLength = 1000;

    char *logPath = nullptr;

    bool fnWriteFile(const char* output);
    void fnBaseLogFunction(enum Status status, char *__format, ...);

    static void fnGetFileName(char* dateString);
    static void createDir();
    static const char* fnGetHeader(enum Status status);
    static char* fnGetTimeString();
    static void fnJointString(char* destination,const char *headString, char* timeString, char* msgString);
    static int getLogLength(const char* log);
};


#endif //CAMEL_SERVER_LOGGER_H
