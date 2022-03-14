#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#include <arpa/inet.h>
#define SIZE 500

struct User
{
    int is_authenticated;
    char uname[SIZE];
};
void acknowledge(int sockfd)
{
    char ack[SIZE];
    bzero(ack, sizeof(ack));
    recv(sockfd, ack, sizeof(ack), 0);
}

double store_file(int connfd, char filename[])
{
    int n;
    FILE *fp;
    char buffer[SIZE];
    bzero(buffer, SIZE);
    fp = fopen(filename, "w");

    time_t start = time(NULL);
    while (1)
    {
        bzero(buffer, SIZE);
        n = recv(connfd, buffer, SIZE, 0);
        char ack[SIZE];
        strcpy(ack, "OK");
        send(connfd, ack, sizeof(ack), 0);

        char str[10];
        strncpy(str, buffer, 9);
        // printf("%s--\n", str);
        if (strncmp(str, "file_sent", 5) == 0)
        {
            printf("**file received**\n");
            break;
        }
        if (strncmp(str, "error", 5) == 0)
        {
            return -1;
        }

        fwrite(buffer, sizeof(char), n, fp);
        bzero(buffer, SIZE);
    }
    time_t end = time(NULL);
    double time_taken = difftime(end, start);
    return time_taken;
}

int get_file(int connfd, char filename[])
{
    int n;
    char data[SIZE] = {0};

    bzero(data, SIZE);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("[-]Error in reading file.");
        bzero(data, SIZE);
        strcpy(data, "error");
        send(connfd, data, sizeof(data), 0);
        acknowledge(connfd);
        return -1;
    }
    while (1)
    {
        int bs = fread(data, sizeof(char), 500, fp);
        if (bs == 0)
        {
            bzero(data, SIZE);
            strcpy(data, "file_sent");
            send(connfd, data, sizeof(data), 0);
            acknowledge(connfd);
            break;
        }
        if (send(connfd, data, bs, 0) == -1)
        {
            printf("[-]Error in sending file.");
            return 1;
        }
        acknowledge(connfd);
        bzero(data, SIZE);
    }
    fclose(fp);
    return 0;
}

void split_msg(char str[], char str1[], char str2[])
{
    bzero(str1, SIZE);
    bzero(str2, SIZE);
    int i = 0;
    while (i < strlen(str))
    {
        if (str[i] == ' ')
            break;
        i++;
    }
    strncpy(str1, str, i);
    if (i < strlen(str) - 1)
        strcpy(str2, str + i + 1);
}
// commence session if user authenticated
void session(struct User user, int connfd)
{
    char str[SIZE], str1[SIZE], str2[SIZE];
    while (1)
    {
        recv(connfd, str, sizeof(str), 0);
        split_msg(str, str1, str2);
        if (strcmp(str1, "CreateFile") == 0)
        {
            FILE *fp = fopen(str2, "w");
            if (fp != NULL)
            {
                strcpy(str, "OK File created.\n");
                send(connfd, str, sizeof(str), 0);
            }
            else
            {
                strcpy(str, "NOTOK File NOT created.\n");
                send(connfd, str, sizeof(str), 0);
            }
            fclose(fp);
        }
        else if (strcmp(str1, "ListDir") == 0)
        {
            DIR *dirp;
            struct dirent *dp;
            bzero(str, sizeof(str));

            dirp = opendir(".");
            char temp[SIZE];
            bzero(temp, sizeof(temp));

            while ((dp = readdir(dirp)))
            {
                if (dp->d_type == DT_REG)
                {
                    sprintf(temp, "%s\n", dp->d_name);
                    strcat(str, temp);
                }
            }
            str[strlen(str)] = '\0';
            send(connfd, str, sizeof(str), 0);
            closedir(dirp);
        }
        else if (strcmp(str1, "StoreFile") == 0)
        {
            char ack[SIZE];
            strcpy(ack, "OK for storefile");
            send(connfd, ack, sizeof(ack), 0);

            double time_taken = store_file(connfd, str2);
            if (time_taken >= 0)
                printf("** File Received sucessfully\ttime taken = %.2f s **\n", time_taken);
        }
        else if (strcmp(str1, "GetFile") == 0)
        {
            int flag = get_file(connfd, str2);
            if (flag == 0)
            {
                strcpy(str, "File Send sucessfully.\n");
                send(connfd, str, sizeof(str), 0);
            }
            else if (flag == -1)
            {
                strcpy(str, "File NOT Found.\n");
                send(connfd, str, sizeof(str), 0);
            }
            else
            {
                strcpy(str, "File NOT Send.\n");
                send(connfd, str, sizeof(str), 0);
            }
        }
        else if (strcmp(str1, "LOGOUT") == 0)
        {
            return;
        }
        else
        {
            strcpy(str, "505 Command not supported.\n");
            send(connfd, str, sizeof(str), 0);
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
    char str[SIZE];

    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }
    else
    {
        strcpy(str, "200 OK Connection is set up.");
        printf("%s\n", str);
        send(connfd, str, sizeof(str), 0);
    }

    // Function for chatting between client and server
    char buff[SIZE];
    struct User user;
    user.is_authenticated = 0;
    while (1)
    {
        bzero(str, sizeof(str));
        recv(connfd, str, sizeof(str), 0);
        // printf("**%s**\n", str);
        char str1[SIZE], str2[SIZE];
        split_msg(str, str1, str2);
        // printf("%s--%s\n", str1, str2);
        if (strcmp(str1, "USERN") == 0)
        {
            FILE *fp = fopen("logincred.txt", "r");
            char uname[SIZE], password[SIZE], udetail[SIZE];
            char *udetail_ptr = udetail;
            size_t udetail_size = sizeof(udetail);
            int flag = 0;
            while (getline(&udetail_ptr, &udetail_size, fp) != -1)
            {
                split_msg(udetail, uname, password);
                if (strcmp(str2, uname) == 0)
                {
                    password[strlen(password) - 1] = '\0';
                    strcpy(buff, "300 Correct Username; Need password\n");
                    send(connfd, buff, sizeof(buff), 0);
                    bzero(str, sizeof(str));
                    recv(connfd, str, sizeof(str), 0);
                    split_msg(str, str1, str2);
                    if (strcmp(str1, "PASSWD") == 0 && strcmp(str2, password) == 0)
                    {
                        char temp[10];
                        strcpy(temp, uname);
                        sprintf(buff, "305 User Authenticated with password\nWelcome %s\n", temp);
                        user.is_authenticated = 1;
                        strcpy(user.uname, uname);
                        send(connfd, buff, sizeof(buff), 0);
                        session(user, connfd);
                        break;
                    }
                    else
                    {
                        strcpy(buff, "310 Incorrect password\n");
                        send(connfd, buff, sizeof(buff), 0);
                    }
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                strcpy(buff, "301 Incorrect Username\n");
                send(connfd, buff, sizeof(buff), 0);
            }
            fclose(fp);
        }
        else if (strcmp(str1, "Bye") == 0)
        {
            printf("[+]Closing the connection.\n");
            close(connfd);
            return 0;
        }
        else
        {
            strcpy(str, "505 Command not supported.\n");
            send(connfd, str, sizeof(str), 0);
        }
        printf("---------\n");
    }

    // commence session if user authenticated

    close(connfd);
    return 0;
}
