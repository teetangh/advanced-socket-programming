// Created by Kaustav Ghosh
// 08 Apr 2020

// FOR WINDOWS
#include <winsock2.h>

// FOR UNIX
// #include <arpa/inet.h>
// #include <netinet/in.h>

#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    int socketfd;

    if (argc < 2)
    {
        fprintf(stderr, "use %s <port> \n", argv[0]);
        exit(-1);
    }
    int portno = atoi(argv[1]);

    socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (!socketfd)
    {
        perror("Error in opening socket\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(portno);

    if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr) < 0))
    {
        perror("failed to bind\n");
        exit(2);
    }

    int optval = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
    struct sockaddr_in clientAddr;
    char recvBuf[2048];
    int bufLen = sizeof(recvBuf);
    int recvLen = 0;
    int cliLen = sizeof(clientAddr);
    while (1)
    {
        cliLen = sizeof(clientAddr);
        if ((recvLen = recvfrom(socketfd, recvBuf, bufLen, 0, (struct sockaddr *)&clientAddr, &cliLen)) < 0)
        {
            perror("Error in checking\n");
            exit(4);
        }

        recvBuf[recvLen] = 0;
        printf(" Recv from %s: %d:\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("%s \n", recvBuf);
        // int bufLen = strlen(buf);
        if (sendto(socketfd, recvBuf, recvLen, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0)
        {
            perror("Error in sending\n");
            exit(3);
        }
    }

    return 0;
}

// gcc udpserver.c -o udpserver.o -lws2_32
// OR
// gcc -o udpserver udpserver.c -lws2_32