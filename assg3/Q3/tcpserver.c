#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#include <arpa/inet.h>
#define SIZE 500

void send_file(int sockfd)
{
    int n;
    char data[SIZE] = {0};
    ;
    bzero(data, SIZE);
    FILE *fp;

    char *filename = "send.mp4";

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }

    printf("*****\n");
    while (1)
    {
        int bs = fread(data, sizeof(char), 500, fp);
        if (bs == 0)
        {
            char str[SIZE] = "file_sent";

            bzero(data, SIZE);
            strcpy(data, "file_sent");
            send(sockfd, str, sizeof(str), 0);
            printf("%s*****\n", str);
            return;
        }
        if (send(sockfd, data, bs, 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, SIZE);
    }
    printf("END*****\n");
    // while (fgets(data, SIZE, fp) != NULL)
    // {
    //     if (send(sockfd, data, strlen(data), 0) == -1)
    //     {
    //         perror("[-]Error in sending file.");
    //         exit(1);
    //     }
    //     bzero(data, SIZE);
    // }
    return;
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");

    // Function for chatting between client and server

    char str[200];
    while (1)
    {
        bzero(str, sizeof(str));
        read(connfd, str, sizeof(str));
        printf("**%s**\n", str);
        if (strcmp(str, "GivemeyourVideo") == 0)
        {
            send_file(connfd);
            printf("[+]File data sent successfully.\n");
        }
        else if (strcmp(str, "Bye") == 0)
        {
            printf("[+]Closing the connection.\n");
            break;
        }
        else
        {
            write(connfd, str, sizeof(str));
        }
        printf("---------\n");
    }

    close(connfd);
    return 0;
}
