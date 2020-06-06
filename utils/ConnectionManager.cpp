//
// Created by Decision on 2020/5/12.
//


#include "ConnectionManager.h"
#include "../camel_server.h"


ConnectionManager::ConnectionManager(int _port, RSA *_rsa, Logger *_logger) :port(_port), connectRSA(_rsa), logger(_logger) {
    nowPath.clear();
}

ConnectionManager::~ConnectionManager() {
    if (ft != nullptr) {
        ft -> stopThread();
        while (true) {
            if (ft -> getCloseStatus()) {
                delete ft;
                logger -> success("File transport thread closed.");
                break;
            }
        }
    }
}

void ConnectionManager::startConnection() {
    int listen_fd, connect_fd, n, statusCode;
    unsigned char recv_buffer[4096], send_buffer[4096], buffer[4096];
    sockaddr_in socketServerStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while create server.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = htons(port);
    socketServerStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port.");
        return ;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d.", port);

    while (true) {
        if ((connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr)) == -1) {
            if (checkTimeout()) {
                logger -> info("Connect timeout on port %d, close socket thread.", port);
                break;
            }
            else continue;
        }
        n = recv(connect_fd, recv_buffer, 4096, 0);
        getStatusCode(statusCode, recv_buffer);
        if (statusCode == SECOND_CONNECT) {
            logger->info("Received authorized request, start auth user.");
            if (authUser(&recv_buffer[2])) {
                logger -> success("User authorized successful, start file transport on port %d.", port);
                generateToken();
                logger -> success("Token generate successful, send to user.");
                putStatusCode(112, send_buffer[0], send_buffer[1]);
                pushValue(buffer, filePort, 2);
                memcpy(&buffer[2], token, 32);
                aesEncrypt(buffer, &send_buffer[2], 34);
                send(connect_fd, send_buffer, 50, 0);
                fileManage(connect_fd);
                break;
            }
            else {
                logger->warning("User authorized failed.");
            }
        }
        else {
            logger -> warning("Received a connect request, but status code %d is error.", statusCode);
        }
        lastTimestamp = time(nullptr);
        close(connect_fd);
    }
    logger -> info("Listen socket closed.");
    close(listen_fd);
}

void ConnectionManager::setUserInfo(char *_username, char *_password) {
    strcpy(username, _username);
    strcpy(password, _password);
}

bool ConnectionManager::authUser(const unsigned char *buffer) {
    char _username[16], _password[16];
    unsigned char plainText[4096];
    int result = RSA_private_decrypt(256, (unsigned char*) buffer, plainText, connectRSA, RSA_PKCS1_PADDING);
    if (result < 0) {
        ERR_print_errors_fp(stdout);
    }
    memcpy(_username, plainText, 16);
    memcpy(_password, &plainText[16], 16);
    if (strcmp(username, _username) != 0 || strcmp(password, _password) != 0) return false;
    memcpy(key, &plainText[32], 32);
    return true;
}

bool ConnectionManager::checkToken(unsigned char buffer[32]) {
    unsigned char recv_token[32];
    aesDecrypt(buffer, recv_token, 32);
    return memcmp(recv_token, token, 32);
}

void ConnectionManager::sendDirInfo(const int &connect_fd) {
    int statusCode, length;
    int infoLength = 0, index = 0, nxtLen = 0;
    unsigned char recv_buffer[4096], send_buffer[4096];
    const char *dirInfoBuffer;
    unsigned char buffer[4096];
    std::string dirInfo;
    FileManager fm(nowPath, logger);
    fm.getDirInfo(dirInfo);
    dirInfoBuffer = dirInfo.c_str();

    infoLength = dirInfo.length();
    logger -> info("Debug info: infoLength = %d", infoLength);
    nxtLen = std::min(infoLength, 4080);
    putStatusCode(100, send_buffer[0], send_buffer[1]);
    memcpy(buffer, dirInfoBuffer, nxtLen);
    pushValue(&send_buffer[2], nxtLen, 2);
    aesEncrypt(buffer, &send_buffer[4], nxtLen);
    send(connect_fd, send_buffer, 4096, 0);
    index = nxtLen;
    while (true) {
        length = recv(connect_fd, recv_buffer, 4096, 0);
        if (length == -1) {
            if (checkTimeout(10)) break;
            continue;
        }
        lastTimestamp = time(nullptr);
        getStatusCode(statusCode, recv_buffer);
        if (statusCode != 402) break;
        if (index != infoLength) {
            nxtLen = std::min(infoLength - index, 4080);
            putStatusCode(100, send_buffer[0], send_buffer[1]);
            pushValue(&send_buffer[2], nxtLen, 2);
            pushValue(send_buffer, nxtLen, 4);
            memcpy(buffer, &dirInfoBuffer[index], nxtLen);
            aesEncrypt(buffer, &send_buffer[4], nxtLen);
            send(connect_fd, send_buffer, 4096, 0);
            index = nxtLen;
        }
        else {
            putStatusCode(504, send_buffer[0], send_buffer[1]);
            send(connect_fd, send_buffer, 2, 0);
            break;
        }
    }
}

void ConnectionManager::fileManage(const int &connect_fd) {
    int statusCode, length;
    unsigned long long len;
    unsigned char recv_buffer[4096], send_buffer[4096], buffer[4096];
    while (true) {
        length = recv(connect_fd, recv_buffer, 4096, 0);
        if (length == -1) {
            if (checkTimeout()) break;
            continue;
        }
        if (checkToken(&recv_buffer[2]) != 0) continue;

        getStatusCode(statusCode, recv_buffer);
        lastTimestamp = time(nullptr);
        switch (statusCode) {
            case REFRESH_DIR: {
                logger -> info("Receive dir information request.");
                sendDirInfo(connect_fd);
                logger -> success("Send dir information successful.");
                break;
            }
            case GOTO_DIR: {
                logger -> info("Receive enter directory request.");
                getValue(&recv_buffer[34], len, 2);
                aesDecrypt(&recv_buffer[36], buffer, len);
                std::string nxtPath = nowPath + "/";
                for (int i = 0;i < len;i++) nxtPath += buffer[i];
                FileManager fm(nxtPath, logger);
                if (! FileManager(nxtPath, logger).checkDirExist()) {
                    sendErrorCode(101, connect_fd);
                }
                else {
                    nowPath = nxtPath;
                    sendDirInfo(connect_fd);
                }
                break;
            }
            case BACK_DIR: {
                logger->info("Back to upper directory.");
                int pos = nowPath.find_last_of('/');
                if (pos == 1) {
                    sendErrorCode(101, connect_fd);
                } else {
                    nowPath = nowPath.substr(0, pos);
                    sendDirInfo(connect_fd);
                }
                break;
            }
            /*
             * 接收到该状态码用于删除目录，但是仅限于当前目录下目录
             */
            case DELETE_DIR: {
                logger -> info("Receive enter directory request.");
                getValue(&recv_buffer[34], len, 2);
                aesDecrypt(&recv_buffer[36], buffer, len);
                std::string nxtPath = nowPath + "/";
                for (int i = 0;i < len;i++) nxtPath += buffer[i];
                FileManager fm(nxtPath, logger);
                if (! fm.checkDirExist()) {
                    sendErrorCode(101, connect_fd);
                }
                else {
                    /*
                    if (fm.deleteDir()) {
                        logger -> success("Delete directory %s successful.", buffer);
                    }
                    else {
                        logger -> error("Some error occurred while delete directory.");
                        sendErrorCode(101, connect_fd);
                    }
                    */
                }
                break;
            }
            case CREATE_DIR: {
                logger -> info("User request to create a directory.");
                getValue(&recv_buffer[34], len, 2);
                aesDecrypt(&recv_buffer[36], buffer, len);
                std::string dirPath = nowPath + '/';
                for(int i = 0; i < len;i++) dirPath.push_back(buffer[i]);
                FileManager fm(dirPath, logger);
                if (fm.createDirectory()) {
                    putStatusCode(104, send_buffer[0], send_buffer[1]);
                    send(connect_fd, send_buffer, 2, 0);
                }
                else {
                    putStatusCode(105, send_buffer[0], send_buffer[1]);
                    send(connect_fd, send_buffer, 2, 0);
                };
                break;
            }
            case CLOSE_CONNECT:
                logger -> info("User request to close connection, close connection.");
                break;
            case POST_FILE:

                break;
            case COPY_FILE:

                break;
            case MOVE_FILE:

                break;
            case DELETE_FILE: {
                break;
            }
        }
        lastTimestamp = time(nullptr);
    }
}

bool ConnectionManager::checkTimeout(long long timeLimit) {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= timeLimit;
}

void ConnectionManager::getStatusCode(int &statusCode, const unsigned char *buffer) {
    statusCode = 0;
    statusCode = (int(buffer[0]) << 8) | (int(buffer[1]));
}

void ConnectionManager::putStatusCode(const int &statusCode, unsigned char &firstChar, unsigned char &secondChar) {
    firstChar =  char((statusCode >> 8) & 0xff);
    secondChar =  char(statusCode & 0xff);
}

void ConnectionManager::generateToken() {
    unsigned char buffer[500];
    long long timestamp = time(nullptr);
    unsigned char *pos = buffer;
    i2d_RSAPublicKey(connectRSA, &pos);
    memcpy(buffer, username, 16);
    memcpy(buffer, password, 16);
    pushValue(&buffer[302], timestamp, 8);
    sha256(buffer, token, 310);
}

void ConnectionManager::sha256(unsigned char *buffer,unsigned char *out, int length) {
    SHA256_CTX _sha256;
    SHA256_Init(&_sha256);
    SHA256_Update(&_sha256, buffer, length);
    SHA256_Final(out, &_sha256);
}

void ConnectionManager::pushValue(unsigned char *destination, unsigned long long value, int bytes_len) {
    for (int i = 1; i <= bytes_len;i++) {
        destination[i - 1] = (value >> ((bytes_len - i) * 8)) & 0xff;
    }
}

void ConnectionManager::setWorkPath(const char *_path) {
    memcpy(path, _path, 32);
    nowPath = "./" + std::string(_path);
}

void ConnectionManager::clearIv() {
    memset(iv, 0, 16);
}

void ConnectionManager::aesEncrypt(const unsigned char *in, unsigned char *out, int len) {
    clearIv();
    AES_set_encrypt_key(key, 256, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_ENCRYPT);
}

void ConnectionManager::aesDecrypt(const unsigned char *in, unsigned char *out, int len) {
    clearIv();
    AES_set_decrypt_key(key, 256, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_DECRYPT);
}

void ConnectionManager::getValue(unsigned char *from, unsigned long long &value, int bytes_len) {
    value = 0;
    for (int i = 0; i < bytes_len; i++) {
        value <<= 8;
        value |= from[i];
    }
}

void ConnectionManager::sendErrorCode(int statusCode, const int &connect_fd) {
    unsigned char buffer[2];
    putStatusCode(statusCode, buffer[0], buffer[1]);
    send(connect_fd, buffer, 2, 0);
}

void ConnectionManager::startFileThread() {
    filePort = chosePort();
    ft = new FileTransport(key, filePort, logger);
    ft -> setToken(token);
    std::thread(&FileTransport::startThread, ft).detach();
    logger -> success("File transport thread starting...");
}

int ConnectionManager::chosePort() {
    srand(time(nullptr));
    int randPort = 0;
    while (!checkPort(randPort)) {
        randPort = rand() % 65535;
    }
    return randPort;
}

bool ConnectionManager::checkPort(int port) {
    if (port < 30000 || port > 65535) return false;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    memset(&sin, 0, sizeof(0));
    sin.sin_family = AF_INET;
    sin.sin_port = port;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socket_fd, (sockaddr*) &sin, sizeof(sockaddr)) < 0) {
        close(socket_fd);
        return false;
    }
    close(socket_fd);
    return true;
}