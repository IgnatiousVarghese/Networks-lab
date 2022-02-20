#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void claculate(char *buff)
{
    int a, b;
    char c = '0';
    sscanf(buff, "%d%c%d", &a, &c, &b);

    switch (c)
    {
    case '+':
        sprintf(buff, "%d + %d = %d", a, b, a + b);
        break;
    case '-':
        sprintf(buff, "%d - %d = %d", a, b, a - b);
        break;
    case '*':
        sprintf(buff, "%d * %d = %d", a, b, a * b);
        break;
    case '/':
        sprintf(buff, "%d / %d = %d", a, b, a / b);
        break;
    default:
        strcpy(buff, "enter valid input");
    }
    printf("%s\n", buff);
}

int main()
{
    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int connfd;
    struct sockaddr_in clientAddr;

    socklen_t addr_size;

    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", PORT);

    if (listen(sockfd, 10) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]Error in binding.\n");
    }

    pid_t childpid;
    while (1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
        if (connfd < 0)
        {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        if ((childpid = fork()) == 0)
        {
            close(sockfd);

            while (1)
            {
                int nb = recv(connfd, buffer, 1024, 0);
                buffer[nb] = '\0';
                if (strncmp(buffer, "exit", 4) == 0 || nb==0)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                    break;
                }
                else
                {
                    printf("Client: %s\n", buffer);
                    claculate(buffer);
                    send(connfd, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
            }

            close(connfd);
        }
    }

    

    return 0;
}