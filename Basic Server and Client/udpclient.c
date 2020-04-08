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

// #include <netdb.h/in.h>
// // void bzero(void *s, size_t n);
// #define bzero(s, n) memset((s), 0, (n))

// // void bcopy(const void *s1, void *s2, size_t n);
// #define bcopy(s1, s2, n) memmove((s2), (s1), (n))

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    struct hostent *server;
    char *hostname;
    int socketfd;

    if (argc < 3)
    {
        fprintf(stderr, "use %s <hostname> <port> \n", argv[0]);
        exit(-1);
    }

    hostname = argv[1];
    int portno = atoi(argv[2]);

    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (!socketfd)
    {
        perror("Error in opening socket\n");
        exit(1);
    }

    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "Can't resolve hostname\n");
        exit(2);
    }

    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "Can't resolve hostname\n");
        exit(2);
    }

    bzero((char *)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
    serverAddr.sin_port = htons(portno);

    char *buf = "Hello there!!\n";
    int bufLen = strlen(buf);
    if (sendto(socketfd, buf, bufLen + 1, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error in sending\n");
        exit(3);
    }

    char recvBuf[2048];
    bufLen = sizeof(recvBuf);
    int recvLen = 0;
    int serLen = sizeof(serverAddr);
    if ((recvLen = recvfrom(socketfd, recvBuf, bufLen, 0, (struct sockaddr *)&serverAddr, &serLen)) < 0)
    {
        recvBuf[recvLen] = 0;
        printf("Recv from %s: %d:\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
        printf(" %s\n", recvBuf);
    }

    return 0;
}

// gcc udpclient.c -o udpclient.o -lws2_32