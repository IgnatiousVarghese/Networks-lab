// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define MAX 80

#define PORT 8080
#define MAXLINE 1024

void func(int connfd, struct sockaddr_in servaddr)
{
    char buff[MAX];
    char fruits[5][20] = {"apple", "manga", "bannana", "chikoo", "papaya"};
    int counts[] = {10, 10, 10, 10, 10};
    int n, len;
    // infinite loop for chat
    for (;;)
    {
        bzero(buff, MAX);

        n = recvfrom(connfd, (char *)buff, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buff[n] = '\0';
        printf("From client: %s \n", buff);
        n = 0;

        if (strcmp(buff, "Fruits") == 0)
        {
            strcpy(buff, "Enter fruit name : ");
            sendto(connfd, buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));

            bzero(buff, MAX);
            n = recvfrom(connfd, (char *)buff, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&servaddr,
                         &len);
            buff[n] = '\0';

            char fruit[10];
            int count;
            sscanf(buff, "%s %d", fruit, &count);

            for (int i = 0; i < 5; i++)
            {
                if (strcmp(fruits[i], fruit) == 0)
                {
                    if (count <= counts[i])
                    {
                        counts[i] -= count;
                        bzero(buff, MAX);
                        sprintf(buff, "%s  left -- %d\n", fruit, counts[i]);
                        sendto(connfd, buff, strlen(buff),
                               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                               sizeof(servaddr));
                        break;
                    }
                    else
                    {
                        bzero(buff, MAX);
                        strcpy(buff, "Not available\n");
                        sendto(connfd, buff, strlen(buff),
                               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                               sizeof(servaddr));
                        break;
                    }
                }
            }
        }

        else if (strcmp(buff, "SendInventory") == 0)
        {
            bzero(buff, MAX);
            for (int i = 0; i < 5; i++)
            {
                strcat(buff, "\t");
                strcat(buff, fruits[i]);
                char s[5];
                sprintf(s, "\t%d\n", counts[i]);
                strcat(buff, s);
            }
            sendto(connfd, buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
        }
        // if msg contains "Exit" then server exit and chat ended.
        else if (strncmp("exit", buff, 4) == 0)
        {
            printf("Server Exit...\n");
            return;
        }

        else
        {
            strcpy(buff, "enter valid input.\n");
            sendto(connfd, buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
        }
    }
}

// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;

    len = sizeof(cliaddr); //len is value/resuslt

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    sendto(sockfd, (const char *)hello, strlen(hello),
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           len);
    printf("Hello message sent.\n");

    func(sockfd, servaddr);

    return 0;
}
