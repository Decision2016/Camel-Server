//
// Created by Decision on 2020/5/11.
//

#include "FileManager.h"

FileManager::FileManager(std::string _path, Logger *_logger):logger(_logger) {
    path = _path;
}

bool FileManager::getDirInfo(std::string &infoString) {
    infoString.clear();
    DIR *dir;
    struct dirent *ptr;
    std::string dirInfo, fileInfo;
    int count = 0;

    if ((dir=opendir(path.c_str())) == nullptr)
    {
        logger -> error("An error occurred while open path %s.", path.c_str());
        return false;
    }

    dirInfo.clear();
    fileInfo.clear();

    while ((ptr = readdir(dir)) != nullptr) {
        if(count != 2) {
            count++;
            continue;
        }
        std::string name(ptr->d_name);
        if(ptr -> d_type == 4) {
            dirInfo += ("folder/" + name + ";");
        }
        else {
            std::string extName;
            getExtName(extName, name);
            fileInfo.append(extName + "/" + name + ";");
        }
    }
    infoString = dirInfo + fileInfo;
}

void FileManager::getExtName(std::string &extName,const std::string &fileName) {
    int pos = fileName.find_last_of('.');
    if (pos == std::string::npos) extName = "other";
    else extName = fileName.substr(pos + 1);

    //todo: extName 筛选
}

bool FileManager::checkDirExist() {
    DIR *dir;
    if ((dir=opendir(path.c_str())) == nullptr) return false;
    return true;
}