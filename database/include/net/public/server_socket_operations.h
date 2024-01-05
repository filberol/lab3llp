#ifndef LLP_SOCKET_OPERATIONS_H
#define LLP_SOCKET_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int initializeServerSocket(const char* addr, int port);

int listenForConnections(int serverSocket);

int acceptConnection(int serverSocket);

int sendData(int socket, const char *data);

int receiveData(int socket, char *buffer);

void closeSocket(int socket);

#endif //LLP_SOCKET_OPERATIONS_H

