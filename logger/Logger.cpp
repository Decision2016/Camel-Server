//
// Created by Decision on 2020/5/7.
//

#include "Logger.h"

void Logger::fnBaseLogFunction(enum Status status, char *__format, ...) {
    const char* header = fnGetHeader(status);
    char *nowTime = fnGetTimeString();
    char destination[dStringLength];
    char log[1024];
    va_list args;
    va_start(args, __format);
    sprintf(log, __format, args);
    va_end(args);
    fnJointString(destination, header, nowTime, log);
        fnWriteFile(destination);
    printf("%s\n", destination);
}

void Logger::error(char *__format, ...) {
    va_list args;
    va_start(args, __format);
    fnBaseLogFunction(Error, __format, args);
    va_end(args);
}

void Logger::success(char *__format, ...) {
    va_list args;
    va_start(args, __format);
    fnBaseLogFunction(Success, __format, args);
    va_end(args);
}

void Logger::info(char *__format, ...) {
    va_list args;
    va_start(args, __format);
    fnBaseLogFunction(Info, __format, args);
    va_end(args);
}

void Logger::warning(char *__format, ...) {
    va_list args;
    va_start(args, __format);
    fnBaseLogFunction(Warning, __format, args);
    va_end(args);
}

const char* Logger::fnGetHeader(enum Status status) {
    switch (status) {
        case Success:
            return "[Success | ";
        case Error:
            return "[Error | ";
        case Info:
            return "[Info | ";
        case Warning:
            return "[Warning | ";
    }
}

void Logger::fnJointString(char *destination, const char *headString, char *timeString, char *msgString) {
    strcpy(destination, headString);
    strcat(destination, timeString);
    strcat(destination, "]");
    strcat(destination, msgString);
}

char* Logger::fnGetTimeString() {
    time_t now = time(nullptr);
    char *resTime = ctime(&now);
    resTime[strlen(resTime) - 1] = 0;
    return resTime;
}

Logger::Logger() : logPath(nullptr) {}

Logger::~Logger() {
    logPath = nullptr;
}

Logger::Logger(char *cLogPath) : logPath(cLogPath) {}

bool Logger::fnWriteFile(const char *output) {
    printf("save\n");
}

char* Logger::fnGetDate() {

}

