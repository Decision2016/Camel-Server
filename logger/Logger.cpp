//
// Created by Decision on 2020/5/7.
//

#include "Logger.h"

#include <cstdlib>
#include <unistd.h>

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
    printf("%s", destination);
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
    strcat(destination, "\n");
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
    char fileName[30], path[200];
    memset(path, 0, sizeof(path));
    memset(fileName, 0, sizeof(fileName));
    fnGetFileName(fileName);
    createDir();
    strcat(path, "./log/");
    strcat(path, fileName);
    FILE* fp = fopen(path, "a");
    fwrite(output, getLogLength(output), 1, fp);
    fclose(fp);
}

void Logger::fnGetFileName(char* dateString) {
    time_t now = time(nullptr);
    tm *gmtm = gmtime(&now);
    sprintf(dateString, "%d-%d-%d", gmtm->tm_year + 1900, gmtm->tm_mon + 1, gmtm->tm_mday);
    strcat(dateString, ".log");
}

void Logger::createDir() {
    if (access("./log", F_OK) >= 0) return ;
    else system("mkdir log");
}


int Logger::getLogLength(const char *log) {
    for(int i = 0; i < 1024 ;i++) {
        if (log[i] == '\n') return i + 1;
    }
    return 1024;
}
