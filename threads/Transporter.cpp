//
// Created by Decision on 2020/6/9.
//

#include "Transporter.h"

Transporter::Transporter(int _port, Logger *_logger) : BaseClass(_port, _logger){}

void Transporter::threadInstance() {
    int n,socket_fd;
    unsigned long long statusCode, length, current;
    socket_fd = accept(listen_fd, (sockaddr*)nullptr, nullptr);
    setConnect(socket_fd);

    if (connect_fd != -1) {
        logger -> info ("Connect with client successful.");
    }

    while(true) {
        if (getThreadStatus()) {
            logger -> info("File transport thread stopping.");
            break;
        }

        length = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);

        if (length == 0) break;

        aesDecrypt(recv_buffer, buffer, BUFFER_LENGTH);
        if (checkToken(&buffer[2]) != 0) continue;

        popValue(buffer, statusCode, STATUS_LENGTH);

        if (statusCode == CONNECTION_END) break;

        switch (statusCode) {
            case FILE_UPLOAD: {
                logger -> info("Receive file upload request, start receive file.");
                popValue(&buffer[34], current, 8);
                popValue(&buffer[42], length, 2);
                std::string destinationPath;
                for (int i = 0; i < length;i++) destinationPath.push_back(buffer[i + 44]);
                recvFile(destinationPath, current);
                break;
            }

            case FILE_DOWNLOAD: {
                logger -> info("Receive file download request, start send file");
                popValue(&buffer[34], current, 8);
                popValue(&buffer[42], length, 2);
                std::string originPath;
                for (int i = 0; i < length; i++) originPath.push_back(buffer[i + 44]);
                sendFile(originPath, current);
                break;
            }
        }
    }
    logger -> info("Close file transport thread on port %d", port);
    close(connect_fd);
    close(listen_fd);
}

void Transporter::recvFile(std::string &_destination, unsigned long long _current) {
    if (_current == 0) remove(_destination.c_str());

    sendStatusCode(SERVER_FILE_RECEIVE);

    int n;
    unsigned long long statusCode, len;
    FILE *fp = fopen(_destination.c_str(), "ab");

    while (true) {
        n = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);

        if (n == -1) continue;

        aesDecrypt(recv_buffer, buffer, BUFFER_LENGTH);
        if (checkToken(&buffer[2]) != 0) continue;

        popValue(buffer, statusCode, 2);

        if (statusCode == FILE_PAUSED || statusCode == CLIENT_FILE_END) {
            fclose(fp);
            logger -> info((char*) "User pause file upload or file receive finished.");
            break;
        }

        if (statusCode == FILE_STOP) {
            fclose(fp);
            remove(_destination.c_str());
            logger -> info((char*) "User stop file upload.");
            break;
        }

        if (statusCode == FILE_TRANSPORT) {
            popValue(&buffer[34], len, 2);
            fwrite(&buffer[36], 1, len, fp);

            sendStatusCode(SERVER_FILE_RECEIVE);
        }
    }
}

void Transporter::sendFile(std::string &_origin, unsigned long long _current) {
    int n;
    unsigned long long statusCode, len;
    unsigned long long current = _current, nxtLen;
    unsigned long long size = getFileSize(_origin);

    pushValue(buffer, SERVER_FILE_TRANSPORT, 2);
    pushValue(&buffer[2], size, 8);
    aesEncrypt(buffer, send_buffer, BUFFER_LENGTH);
    send(connect_fd, send_buffer, BUFFER_LENGTH, 0);

    FILE *fp = fopen(_origin.c_str(), "rb");
    fseeko64(fp, _current, 0);

    while (true) {
        n = recv(connect_fd, recv_buffer, BUFFER_LENGTH, 0);
        if (n == -1) {
            continue;
        }

        aesDecrypt(recv_buffer, buffer, BUFFER_LENGTH);
        if (checkToken(&buffer[2]) != 0) continue;

        popValue(buffer, statusCode, STATUS_LENGTH);

        if (current == size) {
            sendStatusCode(SERVER_FILE_FINISHED);
            break;
        }

        if (statusCode == FILE_PAUSED) break;

        if (statusCode == FILE_STOP) {
            logger -> info((char*) "User stop file download.");
            break;
        }
        pushValue(buffer, SERVER_FILE_TRANSPORT, STATUS_LENGTH);
        nxtLen = std::min(ONCE_MAX_LENGTH, size - current);
        pushValue(&buffer[2], nxtLen, 2);
        fread(&buffer[4], 1, nxtLen, fp);
        aesEncrypt(buffer, send_buffer, BUFFER_LENGTH);
        send(connect_fd, send_buffer, BUFFER_LENGTH, 0);
        current += nxtLen;
    }
    fclose(fp);
}

unsigned long long Transporter::getFileSize(std::string &_path) {
    struct stat statbuf;
    if (stat(_path.c_str(), &statbuf) == 0) {
        return statbuf.st_size;
    }
    return -1;
}