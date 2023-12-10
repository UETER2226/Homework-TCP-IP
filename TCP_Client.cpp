#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<WinSock2.h>
#include<WS2tcip.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
    // Step 1: Inittiate WinSock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2,2);
    if (WSAStartup(wVersion, &wsaData)) {
        printf("Winsock 2.2 is not supported\n");
        return 0;
    }

    //Step 2 : Construct socket
    SOCKET client;
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == INVALID_SOCKET) {
        printf("Error %d: Cannot create server socket.", WSAGetLastError());
        return 0;
    }

    // (optional) Set time-out for receiving
    int tv = 10000; // time-out interval : 10000ms;
    setsockopt(client, SOL_SOCKET, SO_RCVTMEO, (const char*)(&tv), sizeof(int));

    //step 3 : specify server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

    //step 4 : request to connect server 
    if (connect(client, (sockaddr *) &serverAddr, sizeof(serverAddr))) {
        printf("Error %d: Cannot connect server.", WSAGetLastError());
        return 0;
    }
    printf("Connected server!\n");

    //Step 5 : Communicate with server
    char buff[BUFF_SIZE];
    int ret, messageLen;
    while (1) {
        // send message
        printf("Send to server: ");
        gets_s(buff, BUFF_SIZE);
        messageLen = strlen(buff);
        if (messageLen == 0) break;

        ret = send(client, buff, messageLen, 0);
        if (ret == SOCKET_ERROR)
            printf("Error %d: Cannot send data.", WSAGetLastError());

        // Receive echo message
        ret = recv(client, buff, BUFF_SIZE, 0);
        if (ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT)
                printf("Time-out!");
            else printf("Error %d: Cannot receive data.", WSAGetLastError());
        }
        else if (strlen(buff) > 0) {
            buff[ret] = 0;
            printf("Receive from server: %s\n", buff);
        }
    }

    // Step 6 : Close socket
    closessocket(client);

    // Step 7 : Terminate Winsock
    WSACleanup();

    return 0;
}