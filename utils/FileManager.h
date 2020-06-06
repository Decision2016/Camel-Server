//
// Created by Decision on 2020/5/11.
//

#ifndef CAMEL_SERVER_FILEMANAGER_H
#define CAMEL_SERVER_FILEMANAGER_H

#include <string>
#include <map>
#include <dirent.h>
#include <sys/stat.h>

#include "../logger/Logger.h"

class FileManager {
public:
    FileManager(std::string _path, Logger *_logger);

    /*
     * @ params
     * infoString 传入一个用于接收结果的字符串
     * 在读取文件列表信息成功时返回true， 否则返回false
     */
    bool getDirInfo(std::string &infoString);
    bool checkDirExist();
    bool createDirectory();
private:
    std::string path;
    Logger *logger;
    static void getExtName(std::string &extName,const std::string &fileName);
};


#endif //CAMEL_SERVER_FILEMANAGER_H
