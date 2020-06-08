//
// Created by Decision on 2020/5/24.
//

#include "FileTransport.h"

FileTransport::FileTransport(const unsigned char* _key, int _port, Logger *_logger): port(_port), logger(_logger) {
    threadStatus = false;
    memcpy(key, _key, 32);
}

void FileTransport::startThread() {
    int listen_fd, connect_fd, n;
    unsigned long long statusCode, length;
    sockaddr_in socketServerStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while start file transport thread.");
        return ;
    }

    memset(&socketServerStruct, 0, sizeof(sockaddr_in));
    socketServerStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketServerStruct.sin_port = htons(port);
    socketServerStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketServerStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port in file transport thread.");
        return ;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket in file transport thread.");
        return ;
    }
    logger -> info("Socket listener created successful on port %d in file transport thread.", port);
    connect_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr);
    file_socket = connect_fd;

    if (connect_fd != -1) {
        logger -> info ("Connect with client successful.");
    }

    while(true) {
        if(threadStatus == true) {
            logger -> info("file transport thread stopping.");
            break;
        }
        n = recv(connect_fd, recv_buffer, 4096, 0);

        if (n == -1) continue;

        popValue(recv_buffer, statusCode, 2);

        switch (statusCode) {
            case FILE_UPLOAD: {
                logger -> info("Receive file upload request, start receive file.");
                popValue(&recv_buffer[2], length, 2);
                aesDecrypto(&recv_buffer[4], buffer, length);
                std::string destinationPath;
                for (int i = 0; i < length;i++) destinationPath.push_back(buffer[i]);
                recvFile(destinationPath);
                break;
            }
            case FILE_DOWNLOAD: {
                unsigned long long current;
                logger -> info("Receive file download request, start send file");
                popValue(&recv_buffer[2], current, 8);
                popValue(&recv_buffer[10], length, 2);
                aesDecrypto(&recv_buffer[12], buffer, length);
                std::string originPath;
                for (int i = 0; i < length; i++) originPath.push_back(buffer[i]);
                sendFile(originPath, current);
                logger -> success("Send file to client successful.");
                break;
            }
        }
    }
    close(listen_fd);
    closeStatus = false;
}

void FileTransport::recvFile(std::string &_destination) {
    pushValue(send_buffer, SERVER_FILE_UPLOAD, 2);
    send(file_socket, send_buffer, 2, 0);
    int n;
    unsigned long long statusCode, len;

    FILE *fp = fopen(_destination.c_str(), "ab");

    while (true) {
        n = recv(file_socket, recv_buffer, 4096, 0);
        if (n == -1) continue;

        popValue(recv_buffer, statusCode, 2);

        if (statusCode == FILE_PAUSED || statusCode == CLIENT_FILE_END) {
            break;
        }

        if (statusCode == FILE_TRANSPORT) {
            popValue(&recv_buffer[2], len, 2);
            aesDecrypto(&recv_buffer[4], buffer, len);
            fwrite(buffer, 1, len, fp);

            pushValue(send_buffer, SERVER_FILE_UPLOAD, 2);
            send(file_socket, send_buffer, 2, 0);
        }
    }

    fclose(fp);
}

void FileTransport::sendFile(std::string &_origin, unsigned long long _current) {
    unsigned long long statusCode, len;
    unsigned long long current = _current, nxtLen;
    unsigned long long size = getFileSize(_origin);

    pushValue(send_buffer, SERVER_FILE_TRANSPORT, 2);
    pushValue(&send_buffer[2], size, 8);
    send(file_socket, send_buffer, 10, 0);

    FILE *fp = fopen(_origin.c_str(), "rb");
    int n;

    while (true) {
        n = recv(file_socket, recv_buffer, 4096, 0);
        if (n == -1) {
            continue;
        }

        popValue(recv_buffer, statusCode, 2);
        if (statusCode == FILE_PAUSED) break;
        if (current == size) {
            pushValue(send_buffer, SERVER_FILE_FINISHED, 2);
            send(file_socket, send_buffer, 2, 0);
            break;
        }
        pushValue(send_buffer, SERVER_FILE_TRANSPORT, 2);
        nxtLen = std::min(4080ULL, size - current);
        pushValue(&send_buffer[2], nxtLen, 2);
        fread(buffer, 1, nxtLen, fp);
        aesEncrypto(buffer, &send_buffer[4], nxtLen);
        send(file_socket, send_buffer, 4096, 0);
        current += nxtLen;
    }

    fclose(fp);
}

unsigned long long FileTransport::getFileSize(std::string &_path) {
    struct stat statbuf;
    if (stat(_path.c_str(), &statbuf) == 0) {
        return statbuf.st_size;
    }
    return -1;
}

bool FileTransport::checkTimeout() {
    long long delta = time(nullptr) - lastTimestamp;
    return delta >= 600;
}

void FileTransport::stopThread() {
    threadStatus = false;
}

void FileTransport::setToken(unsigned char *buffer) {
    memcpy(token, buffer, 32);
}

bool FileTransport::getCloseStatus() {
    return closeStatus;
}

void FileTransport::pushValue(unsigned char *destination, unsigned long long value, int bytes_len) {
    for (int i = 1; i <= bytes_len;i++) {
        destination[i - 1] = (value >> ((bytes_len - i) * 8)) & 0xff;
    }
}

void FileTransport::popValue(unsigned char *from, unsigned long long &value, int bytes_len) {
    value = 0;
    for (int i = 0; i < bytes_len; i++) {
        value <<= 8;
        value |= from[i];
    }
}

void FileTransport::aesEncrypto(unsigned char *in, unsigned char *out, int len) {
    memset(iv, 0, 16);
    AES_set_encrypt_key(key, 256, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_ENCRYPT);
}

void FileTransport::aesDecrypto(unsigned char *in, unsigned char *out, int len) {
    memset(iv, 0, 16);
    AES_set_decrypt_key(key, 256, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_DECRYPT);
}