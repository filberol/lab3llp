#ifndef LLP_CLIENT_SOCKET_OPERATIONS_H
#define LLP_CLIENT_SOCKET_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_BUFFER_SIZE 1024

int initializeClientSocket();

int connectToServer(int clientSocket, const char *serverAddress, int port);

int sendDataToServer(int clientSocket, const char *data);

int receiveDataFromServer(int clientSocket, char *buffer);

void closeClientSocket(int clientSocket);

#endif //LLP_CLIENT_SOCKET_OPERATIONS_H
