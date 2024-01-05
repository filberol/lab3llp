#include "../../include/net/public/server_socket_operations.h"

#define BACKLOG 10

int initializeServerSocket(const char* serverAddress, int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, serverAddress, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid server IP address");
        close(serverSocket);
        return -1;
    }
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 10) == -1) {
        perror("Error listening on server socket");
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

int listenForConnections(int serverSocket) {
    if (listen(serverSocket, BACKLOG) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        return -1;
    }
    printf("Server listening for connections...\n");
    return 0;
}

int acceptConnection(int serverSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientAddrLen);

    if (clientSocket == -1) {
        perror("Error accepting connection");
        close(serverSocket);
        return -1;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    return clientSocket;
}

int sendData(int socket, const char *data) {
    size_t dataSize = strlen(data);
    if (send(socket, data, dataSize, 0) == -1) {
        perror("Error sending data");
        close(socket);
        return -1;
    }
    return 0;
}

int receiveData(int socket, char *buffer) {
    ssize_t bytesRead = recv(socket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesRead == -1) {
        perror("Error receiving data");
        close(socket);
        return -1;
    }

    buffer[bytesRead] = '\0';
    return 0;
}

void closeSocket(int socket) {
    close(socket);
}

