#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
    // step 1 : initiate Winsock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2,2);
    if (WSAStartup(wVersion, &wsaData)) {
        printf("Winsock 2.2 is not supported\n");
        return 0;
    }

    //step 2 : construct socket
    SOCKET listenSock;
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        printf("Error %d: Cannot create server socket.", WSAGetLastError());
        return 0;
    }

    // step 3 : Bind address to socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

    if (bind(listenSock, (sockaddr *) &serverAddr, sizeof(serverAddr)))
    {
        printf("(Error: %d)Cannot associate a local address with server socket.", WSAGetLastError());
        return 0;
    }

    //step 4 : listen request from client
    if (listen(listenSock, 10)) {
        printf("(Error: %d)Cannot place server ssocket in state LISTEN.", WSAGetLastError());
        return 0;
    }

    printf("Server started!\n");

    //step 5: communicate with client
    sockaddr_in clientAdrr;
    char buff[BUFF_SIZE], clientIP[INET_ADDRSTRLEN];
    int ret, clientAdrrLen = sizeof(clientAdrr), clientPort;
    SOCKET connSock;
    // accept request
    connSock = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrLen);
    if (connSock == SOCKET_ERROR) {
        printf("(Error: %d)Cannot permit incoming connection.", WSAGetLastError());
        return 0;
    }
    else {
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
        clientPort = ntohs(clientAddr.sin_port);
        printf("Accept incoming connection from %s: %d\n", clientIP, clientPort);
    }

    while (1) {
        // receive message from client
        ret = recv(connSock, buff, BUFF_SIZE, 0);
        if (ret == SOCKET_ERROR) {
            printf("Error %d: Cannot receive data.", WSAGetLastError());
            break;
        }
        else if (ret == 0) {
            printf("Client disconnects.\n");
            break;
        }
        else {
            buff[ret] = 0;
            printf("Receive from client[%s : %d]  %s\n", clientIP, clientPort, buff);
            //echo to client
            ret = send(connSock, buff, strlen(buff), 0);
            if (ret == SOCKET_ERROR) {
                printf("Error %d: Cannot send data.\n", WSAGetLastError());
                break;
            }
        } 
    }    // end communicating

    //step 5 : close socket
    closesocket(connSock);
    closesocket(listenSock);

    //step 6 : terminate Winsock
    WSACleanup();

    return 0;
}