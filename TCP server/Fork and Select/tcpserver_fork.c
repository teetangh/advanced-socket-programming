// Created by Kaustav Ghosh
// 08 Apr 2020

// FOR WINDOWS
// #include <winsock2.h>

// FOR UNIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <sys/socket.h>
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

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (!socketfd)
    {
        perror("Error in opening socket\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(portno);

    if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("failed to bind\n");
        exit(2);
    }

    if (listen(socketfd, 5))
    {
        perror("Error in listen: ");
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

        int childfd = accept(socketfd, (struct sockaddr *)&clientAddr, &cliLen);
        if (childfd < 0)
        {
            perror("Error in accept");
            continue;
        }
        printf("New Connection from %s: %d: \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        int forkRet = fork();
        if (forkRet == 0)
        {
            while (1)
            {
                if ((recvLen = recv(childfd, recvBuf, bufLen, 0)) < 0)
                {
                    perror("Error in receiving\n");
                    break;
                }
                if (recvLen == 0)
                {
                    close(childfd);
                    break;
                }

                recvBuf[recvLen] = 0;
                printf("recv: %s\n", recvBuf);
                if (send(childfd, recvBuf, recvLen, 0) < 0)
                {
                    perror("error in sending");
                    continue;
                }
            }
            exit(0);
        }
    }
    return 0;
}

/* WINDOWS */
// For object File
// gcc tcpserver_fork.c -o tcpserver_fork.o -lws2_32

// For Executable File
// gcc -o tcpserver_fork tcpserver_fork.c -lws2_32

/* UNIX */
// gcc tcpserver_fork.c -o tcpserver_fork
//  ./tcpserver_fork 2555