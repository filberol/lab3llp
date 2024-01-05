#include "client_socket_operations.h"

int initializeClientSocket() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating parser socket");
        return -1;
    }
    return clientSocket;
}

int connectToServer(int clientSocket, const char *serverAddress, int port) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the database");
        close(clientSocket);
        return -1;
    }
    return 0;
}

int sendDataToServer(int clientSocket, const char *data) {
    size_t dataSize = strlen(data);
    if (send(clientSocket, data, dataSize, 0) == -1) {
        perror("Error sending data to the database");
        close(clientSocket);
        return -1;
    }
    return 0;
}

int receiveDataFromServer(int clientSocket, char *buffer) {
    ssize_t bytesRead = recv(clientSocket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesRead == -1) {
        perror("Error receiving data from the database");
        close(clientSocket);
        return -1;
    }
    buffer[bytesRead] = '\0';
    return 0;
}

void closeClientSocket(int clientSocket) {
    close(clientSocket);
}
