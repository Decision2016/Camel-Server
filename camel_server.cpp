//
// Created by Decision on 2020/6/9.
//

#include "camel_server.h"

camel_server::camel_server(char *_username, char *_password, Logger *_logger, int _port): BaseClass(_port, _logger) {
    strcpy(username, _username);
    strcpy(password, _password);
}

void camel_server::serverInstance() {
    int socket_fd,length;
    unsigned long long statusCode;
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    while (true) {
        socket_fd = accept(listen_fd,(struct sockaddr *) &client_addr, &len);
        if (socket_fd == -1) continue;
        logger -> info("Receive connect request from ip: %s", inet_ntoa(client_addr.sin_addr));
        recv(socket_fd, recv_buffer, BUFFER_LENGTH, 0);

        popValue(recv_buffer, statusCode, STATUS_LENGTH);

        if (statusCode == FIRST_CONNECT) {
            int randPort = choosePort();
            RSA* keyPair = RSA_new();
            int res = 0;
            BIGNUM* bignum = BN_new();
            res = BN_set_word(bignum, RSA_F4);
            res = RSA_generate_key_ex(keyPair, 2048, bignum, NULL);

            if (res != 1) {
                logger->error("Some error occurred while generate RSA key pair.");
                continue;
            }

            Session se(randPort, keyPair, logger);
            se.setUserInfo(username, password);
            se.setWorkPath(path);
            bool seStatus = se.trySocket();
            logger -> info("Session thread socket status: %s", seStatus == true ? "success" : "error");
            se.setPortLimit(lowerPort, higherPort);
            std::thread(&Session::threadInstance, &se).detach();
            pushValue(send_buffer, SERVER_FIRST_CONNECT, STATUS_LENGTH);
            pushValue(&send_buffer[STATUS_LENGTH], randPort, 2);

            long long timestamp = time(nullptr);
            unsigned char *pubKey = buffer;
            i2d_RSAPublicKey(keyPair, &pubKey);
            pushValue(&buffer[270], timestamp, 8);
            memcpy(&send_buffer[4], buffer, 278);
            sha256(buffer, &send_buffer[282], 278);

            send(socket_fd, send_buffer, BUFFER_LENGTH, 0);
            logger -> success("One file transport thread start running on port %d", randPort);
        }
        close(socket_fd);
    }
    close(listen_fd);
}

bool camel_server::setWorkPath(std::string _path) {
    if (_path.length() != 0) path = _path;
    FileManager fm(_path, logger);
    if (fm.checkDirExist()) return true;
    return fm.createDirectory();
}