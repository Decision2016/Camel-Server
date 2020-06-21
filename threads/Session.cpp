//
// Created by Decision on 2020/6/9.
//

#include "Session.h"

Session::Session(int port, RSA *_rsa, Logger *_logger) : BaseClass (port, _logger){
    keyPair = _rsa;
}

Session::~Session() {
    if (tp) {
        tp -> stopThread();
    }
}

void Session::setUserInfo(const std::string &_username, const std::string &_password) {
    username = _username;
    password = _password;
}

void Session::fileManage() {
    unsigned long long statusCode, length;
    int len;
    while (true) {
        len = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);

        if (len == 0) break;
        aesDecrypt(recv_buffer, buffer, BUFFER_LENGTH);

        popValue(buffer, statusCode, STATUS_LENGTH);

        if (statusCode == CONNECTION_END) break;

        switch (statusCode) {
            case REFRESH_DIR: {
                logger -> info("Receive dir information request.");
                sendDirInfo();
                logger -> success("Send dir information successful.");
                break;
            }
            case ENTER_DIR: {
                logger -> info("Receive enter directory request.");
                popValue(&buffer[34], length, 2);
                std::string nxtPath = nowPath + "/";
                for (int i = 0;i < length;i++) nxtPath += buffer[i + 36];
                FileManager fm(nxtPath, logger);
                if (! FileManager(nxtPath, logger).checkDirExist()) {
                    sendStatusCode(SERVER_NOT_EXISTED);
                }
                else {
                    nowPath = nxtPath;
                    dirLevel ++;
                    sendStatusCode(SERVER_DIR_INFO);
                }
                break;
            }
            case BACKUP_DIR: {
                logger->info("Back to upper directory.");
                int pos = nowPath.find_last_of('/');
                if (dirLevel == 0) {
                    sendStatusCode(SERVER_NOT_EXISTED);
                }
                else {
                    nowPath = nowPath.substr(0, pos);
                    dirLevel --;
                    sendStatusCode(SERVER_DIR_INFO);
                }
                break;
            }
            case DELETE_DIR: {
                logger -> info("Receive enter directory request.");
                popValue(&buffer[34], length, 2);
                std::string deletePath, dirName;
                dirName.clear();
                for (int i = 0;i < length;i++) dirName += buffer[i + 36];
                deletePath = nowPath + '/' + dirName;
                FileManager fm(deletePath, logger);
                if (! fm.checkDirExist()) {
                    sendStatusCode(SERVER_NOT_EXISTED);
                }
                else {
                    if (fm.deleteDirectory()) {
                        logger -> success("Delete directory %s successful.", dirName.c_str());
                        sendStatusCode(SERVER_DELETE);
                    }
                    else {
                        logger -> error("Some error occurred while delete directory.");
                        sendStatusCode(SERVER_NOT_EXISTED);
                    }
                }
                break;
            }
            case CREATE_DIR: {
                logger -> info("User request to create a directory.");
                popValue(&buffer[34], length, 2);
                std::string dirPath = nowPath + '/';
                for(int i = 0; i < length;i++) dirPath.push_back(buffer[i + 36]);
                FileManager fm(dirPath, logger);
                if (fm.createDirectory()) {
                    sendStatusCode(SERVER_EXISTED);
                }
                else {
                    sendStatusCode(SERVER_DIR_CREATED);
                };
                break;
            }
            case RENAME_DIR_FILE: {
                std::string recvString, originName, newName;
                popValue(&buffer[34], length, 2);
                for(int i = 0; i < length;i++) recvString.push_back(buffer[i + 36]);
                int pos = recvString.find('/');
                originName = recvString.substr(0, pos);
                newName = recvString.substr(pos + 1);
                FileManager fm(nowPath, logger);
                if (fm.rename(originName, newName)) {
                    sendStatusCode(SERVER_RENAME_ERROR);
                }
                else {
                    sendStatusCode(SERVER_RENAME);
                }
                break;
            }
            case FILE_GET_PATH: {
                pushValue(buffer, SERVER_FILE_PATH, 2);
                pushValue(&buffer[2], nowPath.length(), 2);
                memcpy(&buffer[4], nowPath.c_str(), nowPath.length());
                aesEncrypt(buffer, send_buffer, BUFFER_LENGTH);
                send(connect_fd, send_buffer, BUFFER_LENGTH, 0);
                break;
            }
            case FILE_DELETE: {
                logger -> info("Receive delete file request.");
                popValue(&buffer[34], length, 2);
                std::string deletePath, fileName;
                fileName.clear();
                for (int i = 0;i < length;i++) fileName += buffer[i + 36];
                deletePath = nowPath + '/' + fileName;
                FileManager fm(deletePath, logger);

                if (fm.deleteFile()) {
                    sendStatusCode(SERVER_DELETE);
                }
                else {
                    sendStatusCode(SERVER_NOT_EXISTED);
                }
                break;
            }
        }
    }
    close(connect_fd);
}

void Session::threadInstance() {
    int socket_fd, n;
    unsigned long long statusCode, length;
    unsigned char _token[TOKEN_LENGTH];
    unsigned char test1[BUFFER_LENGTH], test2[BUFFER_LENGTH];

    socket_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr);
    setConnect(socket_fd);
    while (true) {
        n = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);
        if (n == -1) continue;
        if (n == 0) {
            logger -> info("Socket client break connection, close session thread.");
            break;
        }
        popValue(recv_buffer, statusCode, STATUS_LENGTH);

        if (statusCode == SECOND_CONNECT) {
            logger->info("Received authorized request, start auth user.");
            if (authUser(&recv_buffer[2])) {
                logger -> success("User authorized successful, start file transport on port %d.", port);
                generateToken(_token);
                setToken(_token);
                logger -> success("Token generate successful, send to user.");
                int filePort = startFileThread();

                pushValue(send_buffer,SERVER_SECOND_CONNECT, STATUS_LENGTH);
                pushValue(buffer, filePort, 2);
                memcpy(&buffer[2], _token, 32);
                aesEncrypt(buffer, &send_buffer[2], 34);
                send(connect_fd, send_buffer, 50,  0);
                fileManage();
                break;
            }
            else {
                logger -> error("Receive username or password is wrong.");
                sendStatusCode(SERVER_PASS_ERROR);
                continue;
            }
        }
    }
    close(listen_fd);
    logger -> info("Close session thread on port %d", port);
}

void Session::sendDirInfo() {
    unsigned long long statusCode;
    int length;
    int infoLength = 0, index = 0, nxtLen = 0;
    const char *dirInfoBuffer;
    std::string dirInfo;
    FileManager fm(nowPath, logger);
    fm.getDirInfo(dirInfo);
    dirInfoBuffer = dirInfo.c_str();

    infoLength = dirInfo.length();
    // logger -> info("Debug info: infoLength = %d", infoLength);
    nxtLen = std::min(infoLength, (int)ONCE_MAX_LENGTH);

    pushValue(buffer, SERVER_DIR_INFO, STATUS_LENGTH);
    pushValue(&buffer[2], nxtLen, 2);
    memcpy(&buffer[4], dirInfoBuffer, nxtLen);

    aesEncrypt(buffer, send_buffer, BUFFER_LENGTH);
    send(connect_fd, send_buffer, BUFFER_LENGTH, 0);
    index = nxtLen;

    while (true) {
        length = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);
        if (length == -1) {
            continue;
        }

        aesDecrypt(recv_buffer, buffer, BUFFER_LENGTH);
        popValue(buffer, statusCode, STATUS_LENGTH);

        if (statusCode != RECEIVE_SUCCESS) break;
        if (index != infoLength) {
            nxtLen = std::min(infoLength - index, (int)ONCE_MAX_LENGTH);

            pushValue(buffer, SERVER_DIR_INFO, STATUS_LENGTH);
            pushValue(&buffer[2], nxtLen, 2);
            memcpy(&buffer[4], dirInfoBuffer, nxtLen);

            aesEncrypt(buffer, send_buffer, nxtLen);
            send(connect_fd, send_buffer, BUFFER_LENGTH, 0);
            index += nxtLen;
        }
        else {
            logger -> success("Send directory information finish.");
            sendStatusCode(SERVER_INFO_END);
            break;
        }
    }
}

int Session::startFileThread() {
    int filePort = choosePort();
    tp = new Transporter(filePort, logger);
    tp -> setKey(key);
    tp -> setToken(token);
    bool tpStatus = tp -> trySocket();
    logger -> info("Transport thread on port %d status is: %s", filePort, tpStatus ? "Success" : "Error");
    std::thread(&Transporter::threadInstance, tp).detach();
    return filePort;
}

bool Session::authUser(const unsigned char *buffer) {
    char _username[USERNAME_LENGTH], _password[PASSWORD_LENGTH];
    unsigned char plainText[BUFFER_LENGTH];
    int result = RSA_private_decrypt(256, (unsigned char*) buffer, plainText, keyPair, RSA_PKCS1_PADDING);
    if (result < 0) return false;
    memcpy(_username, plainText, USERNAME_LENGTH);
    memcpy(_password, &plainText[USERNAME_LENGTH], PASSWORD_LENGTH);
    if (username != std::string(_username) || password != std::string(_password)) return false;
    setKey(&plainText[64]);
    return true;
}

void Session::setWorkPath(std::string _path) {
    nowPath = _path;
}

void Session::generateToken(unsigned char* out) {
    unsigned char buffer[500];
    long long timestamp = time(nullptr);
    unsigned char *pos = buffer;
    i2d_RSAPublicKey(keyPair, &pos);
    memcpy(buffer, username.c_str(), 16);
    memcpy(buffer, password.c_str(), 16);
    pushValue(&buffer[302], timestamp, 8);
    sha256(buffer, out, 310);
}