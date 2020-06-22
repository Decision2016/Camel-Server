//
// Created by Decision on 2020/6/9.
//

#include "BaseClass.h"

BaseClass::BaseClass(int _port, Logger *_logger): port(_port), logger(_logger) {}

int BaseClass::trySocket(bool isRand) {
    if (isRand) {
        setPort(choosePort());
    }

    sockaddr_in socketStruct;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger -> error("An error occurred while create server.");
        return -1;
    }

    memset(&socketStruct, 0, sizeof(sockaddr_in));
    socketStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    socketStruct.sin_port = htons(port);
    socketStruct.sin_family = AF_INET;

    if (bind(listen_fd, (sockaddr*) &socketStruct, sizeof(sockaddr)) == -1) {
        logger -> error("An error occurred while bind port.");
        return -1;
    }

    if (listen(listen_fd, 10) == -1) {
        logger -> error("An error occurred while listen socket.");
        return -1;
    }

    logger -> info("Socket listener created successful on port %d", port);

    return port;
}

void BaseClass::setConnect(int _connect_fd) {
    connect_fd = _connect_fd;
}

void BaseClass::setPort(int _port) {
    port = _port;
}

void BaseClass::setKey(unsigned char *_key) {
    memcpy(key, _key, KEY_LENGTH);
}

void BaseClass::setToken(unsigned char *_token) {
    memcpy(token, _token, TOKEN_LENGTH);
}

void BaseClass::sha256(unsigned char *buffer,unsigned char *out, int length) {
    SHA256_CTX _sha256;
    SHA256_Init(&_sha256);
    SHA256_Update(&_sha256, buffer, length);
    SHA256_Final(out, &_sha256);
}

void BaseClass::aesEncrypt(unsigned char *in, unsigned char *out, int len) {
    memset(iv, 0, IV_LENGTH);
    AES_set_encrypt_key(key, AES_KEY_LENGTH, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_ENCRYPT);
}

void BaseClass::aesDecrypt(unsigned char *in, unsigned char *out, int len) {
    memset(iv, 0, IV_LENGTH);
    AES_set_decrypt_key(key, AES_KEY_LENGTH, &aesKey);
    AES_cbc_encrypt(in, out, len, &aesKey, iv, AES_DECRYPT);
}

void BaseClass::sendStatusCode(unsigned long long _statusCode) {
    unsigned char buffer[BUFFER_LENGTH];
    pushValue(_buffer, _statusCode, STATUS_LENGTH);
    aesEncrypt(_buffer, buffer, BUFFER_LENGTH);
    send(connect_fd, buffer, BUFFER_LENGTH, 0);
}

void BaseClass::stopThread() {
    threadStatus = false;
}

bool BaseClass::getThreadStatus() {
    return threadStatus;
}

void BaseClass::pushValue(unsigned char *destination, unsigned long long value, int bytes_len) {
    for (int i = 1; i <= bytes_len;i++) {
        destination[i - 1] = (value >> ((bytes_len - i) * 8)) & 0xff;
    }
}

void BaseClass::popValue(unsigned char *origin, unsigned long long &value, int bytes_len) {
    value = 0ULL;
    for (int i = 0; i < bytes_len; i++) {
        value <<= 8;
        value |= origin[i];
    }
}

bool BaseClass::checkToken(unsigned char *buffer) {
    return memcmp(buffer, token, TOKEN_LENGTH);
}

int BaseClass::choosePort() {
    srand(time(nullptr));
    int randPort = 0;
    while (!checkPort(randPort)) {
        randPort = rand() % 65535;
    }
    return randPort;
}

bool BaseClass::checkPort(int port) {
    return !(port < lowerPort || port > higherPort);
}

void BaseClass::setPortLimit(int _low, int _high) {
    lowerPort = _low;
    higherPort = _high;
}