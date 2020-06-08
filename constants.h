//
// Created by Decision on 2020/6/7.
//

#ifndef CAMEL_SERVER_CONSTANTS_H
#define CAMEL_SERVER_CONSTANTS_H

const int REFRESH_DIR = 200;
const int ENTER_DIR = 201;
const int BACKUP_DIR = 202;
const int DELETE_DIR = 203;
const int CREATE_DIR = 204;
const int RENAME_DIR_FILE = 205;
const int FIRST_CONNECT = 210;
const int SECOND_CONNECT = 211;
const int CONNECTION_END = 212;
const int FILE_UPLOAD = 220;
const int CLIENT_FILE_END = 221;
const int FILE_DELETE = 222;
const int FILE_GET_PATH = 223;
const int FILE_PAUSED = 225;
const int FILE_DOWNLOAD = 226;
const int FILE_STOP = 227;
const int FILE_TRANSPORT = 228;

const int SERVER_DIR_INFO = 100;
const int SERVER_DIR_NOT_EXISTED = 101;
const int SERVER_DELETE = 102;
const int SERVER_DIR_CREATED = 104;
const int SERVER_DIR_EXISTED = 105;
const int SERVER_RENAME = 131;
const int SERVER_RENAME_ERROR = 132;
const int SERVER_FILE_UPLOAD = 133;
const int SERVER_FILE_TRANSPORT = 136;
const int SERVER_FILE_FINISHED = 137;
const int SERVER_FILE_PATH = 138;
const int SERVER_FILE_NOT_EXISTED = 139;

#endif //CAMEL_SERVER_CONSTANTS_H
