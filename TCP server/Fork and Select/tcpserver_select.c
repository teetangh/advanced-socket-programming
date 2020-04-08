// Created by Kaustav Ghosh
// 08 Apr 2020

// FOR WINDOWS
// #include <winsock2.h>

// FOR UNIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h> //for fd_set

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

    int fds[16];
    int fdCapa = 16;
    int fdlen = 0;

    fd_set rfds;
    int maxfd = socketfd;
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(socketfd, &rfds);
        int x = 0;
        maxfd = socketfd;

        for (x = 0; x < fdlen; x++)
        {
            FD_SET(fds[x], &rfds);
            if (maxfd < fds)
                maxfd = fds[x];
        }

        select(maxfd + 1, &rfds, NULL, NULL, NULL);

        if (FD_ISSET(socketfd, &rfds))
        {
            cliLen = sizeof(clientAddr);
            int childfd = accept(socketfd, (struct sockaddr *)&clientAddr, &cliLen);
            if (childfd < 0)
            {
                perror("Error in Accepting");
                continue;
            }

            printf("New Connection from %s: %d: \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            if (fdlen == fdCapa)
                close(childfd);
            else
            {
                fds[fdlen] = childfd;
                fdlen++;
            }
        }
        else
        {
            int childfd = 0;
            for (x = 0; x < fdlen; x++)
            {
                if (!FD_ISSET(fds[x], &rfds))
                    continue;
                childfd = fds[x];
                if ((recvLen = recv(childfd, recvBuf, bufLen, 0)) < 0)
                {
                    perror("Error in receiving");
                    FD_CLR(childfd, &rfds);
                    fds[x] = fds[fdlen - 1];
                    fdlen--;
                    break;
                }
                if (recvLen == 0)
                {
                    close(childfd);
                    FD_CLR(childfd, &rfds);
                    fds[x] = fds[fdlen - 1];
                    fdlen--;
                    printf("Closing \n");
                    break;
                }

                recvBuf[recvLen] = 0;
                printf("recv: %s \n", recvBuf);
                if (send(childfd, recvBuf, recvLen, 0) < 0)
                {
                    perror("error in sending");
                    break;
                }
            }
        }
    }

    return 0;
}

/* WINDOWS */
// For object File
// gcc tcpserver_select.c -o tcpserver_select.o -lws2_32

// For Executable File
// gcc -o tcpserver_select tcpserver_select.c -lws2_32

/* UNIX */
// gcc tcpserver_select.c -o tcpserver_select
//  ./tcpserver_select 2444