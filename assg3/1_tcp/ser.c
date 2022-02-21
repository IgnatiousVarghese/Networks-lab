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
   
// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    char fruits[5][20]  = { "apple", "manga", "bannana", "chikoo", "papaya"};
    int counts[] = {10, 10, 10, 10, 10};
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        read(connfd, buff, sizeof(buff));
        printf("From client: %s \n", buff);
        n = 0;


        if(strcmp(buff, "Fruits") == 0)
        {
            strcpy(buff, "Enter fruit name : ");
            write(connfd, buff, sizeof(buff));

            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));

            char fruit[10];
            int count;
            sscanf(buff, "%s %d", fruit, &count);

            for(int i =0; i<5; i++)
            {
                if( strcmp(fruits[i], fruit) == 0 )
                {
                    if(count <= counts[i])
                    {
                        counts[i] -= count;
                        bzero(buff, MAX);
                        sprintf(buff, "%s  left -- %d\n", fruit, counts[i]);
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                    else
                    {
                        bzero(buff, MAX);
                        strcpy(buff, "Not available\n");
                        write(connfd, buff, sizeof(buff));
                        break;
                    }
                }
            }
        }

        else if(strcmp(buff, "SendInventory") == 0)
        {
            bzero(buff, MAX);
            for(int i =0; i<5; i++)
            {
                strcat(buff, "\t");
                strcat(buff, fruits[i]);
                char s[5];
                sprintf(s, "\t%d\n", counts[i]);
                strcat(buff, s);
            }
            write(connfd, buff, sizeof(buff));
        }
        // if msg contains "Exit" then server exit and chat ended.
        else if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            return;
        }

        else
        {
            strcpy(buff, "enter valid input.\n");
            write(connfd, buff, sizeof(buff));
        }
   
        
    }
}
   
// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
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
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}
