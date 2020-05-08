//
// Created by Decision on 2020/5/7.
//

#include "Logger.h"

void Logger::fnBaseLogFunction(enum Status status, char *sMsg, bool saveToFile) {
    const char* header = fnGetHeader(status);
    char *nowTime = fnGetTimeString();
    char destination[dStringLength];
    fnJointString(destination, header, nowTime, sMsg);
    if(saveToFile) {
        fnWriteFile(destination);
    }
    printf("%s\n", destination);
}

void Logger::error(char *sMsg, bool saveToFile) {
    fnBaseLogFunction(Error, sMsg, saveToFile);
}

void Logger::success(char *sMsg, bool saveToFile) {
    fnBaseLogFunction(Success, sMsg, saveToFile);
}

void Logger::info(char *sMsg, bool saveToFile) {
    fnBaseLogFunction(Info, sMsg, saveToFile);
}

void Logger::warning(char *sMsg, bool saveToFile) {
    fnBaseLogFunction(Warning, sMsg, saveToFile);
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