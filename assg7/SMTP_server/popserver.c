#include "validate.c"
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
#define SIZE 500
#define XSIZE 5000
#define SA struct sockaddr
#include <arpa/inet.h>

int PORT;
struct user
{
    char uname[30];
    char passwd[30];
};

struct mail
{
    char data[XSIZE];
};
void acknowledge(int sockfd)
{
    char ack[SIZE];
    bzero(ack, sizeof(ack));
    recv(sockfd, ack, sizeof(ack), 0);
    printf("[ACK] %s", ack);
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
void get_time_string(char *time_string)
{
    time_t current_time;
    struct tm *timeinfo;

    current_time = time(NULL);
    timeinfo = localtime(&current_time);
    strftime(time_string, 100, "%a, %d %b %Y %I:%M:%S Localtime", timeinfo);
}

void display_mails(int sockfd, struct mail *mails, int total_mails)
{
    char msg[XSIZE], str[SIZE];
    bzero(msg, sizeof(msg));
    bzero(str, sizeof(str));
    printf("All mails in inbox are:\n");
    for (int i = 0; i < total_mails; i++)
    {
        char from[100], to[100], subject[100], received[100], body[XSIZE];
        get_field_pop3(mails[i].data, "From", from);
        get_field_pop3(mails[i].data, "To", to);
        get_field_pop3(mails[i].data, "Subject", subject);
        get_field_pop3(mails[i].data, "Received", received);
        get_field_pop3(mails[i].data, "Body", body);
        sprintf(str, "%d. <%s> <received time:%s> <d %s>\n", i+1, from, received, subject);
        // printf("%s", str);
        strcat(msg, str);
    }
    send(sockfd, msg, sizeof(msg), 0);
    // acknowledge(sockfd);
}
int read_all_mails(int sockfd, struct user User, struct mail *mails)
{
    char mailbox_fname[100], mail_txt[XSIZE];
    sprintf(mailbox_fname, "%s/mymailbox.mail", User.uname);
    printf("filename : --%s--\n", mailbox_fname);
    FILE *mailbox = fopen(mailbox_fname, "r");
    int total_mails = 0;
    int i = 0; // to point array of mails
    while (1)
    {
        int flag = 0;
        char str[100];
        while (1)
        {
            if (fscanf(mailbox, "%[^\n]%*c", str) == EOF)
            {
                printf("[+] completed reading read full inbox of %s.\n", User.uname);
                flag = 1;
                break;
            }
            strcat(mail_txt, str);
            strcat(mail_txt, "\n");
            if (strcmp(str, ".") == 0)
            {
                // printf("-->>%s\n", mail_txt);
                strcpy(mails[i].data, mail_txt);
                bzero(mail_txt, sizeof(mail_txt));
                i++;
                total_mails++;
                break;
            }
        }
        if (flag == 1)
            break;
    }
    return total_mails;
}
void session_pop3(int sockfd, struct user User, struct user *Users, int count)
{

    struct mail *mails = (struct mail *)malloc(100 * sizeof(struct mail)); // array of mail stucts
    int total_mails = read_all_mails(sockfd, User, mails);
    display_mails(sockfd, mails, total_mails);

    char str[SIZE];
    while (1)
    {
        bzero(str, sizeof(str));
        recv(sockfd, str, sizeof(str), 0);
        printf("--%s--\n", str);
        if (str[0] == 'd')
        {
            int n;
            sscanf(str, "%*s%d", &n);
            if (n > 0 && n <= total_mails)
            {
                char mailbox_fname[100] = {0};
                sprintf(mailbox_fname, "%s/mymailbox.mail", User.uname);
                printf("filename to del : --%s--\n", mailbox_fname);
                FILE *mailbox ;
                mailbox = fopen(mailbox_fname, "w");
                fclose(mailbox);
                mailbox = fopen(mailbox_fname, "a");
                // write into mailbox excluding deleted mail
                for(int i = 0; i<total_mails; i++)
                {
                    if(i == n-1)
                        continue;
                    fprintf(mailbox, "%s", mails[i].data);
                }
                fclose(mailbox);
            }
            printf("delete %d th mail\n", n);
            total_mails = read_all_mails(sockfd, User, mails);
            display_mails(sockfd, mails, total_mails);
        }
        else if (str[0] == 'q')
        {
            printf("-----------------\nGoodBye\n--------------\n");
            break;
        }
        else
        {
            display_mails(sockfd, mails, total_mails);
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        PORT = atoi(argv[1]);
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    if (1) // socket create and verification
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
            printf("[+] Pop3 sever Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
        {
            printf("[-] socket bind failed...\n");
            exit(0);
        }
        else
            printf("[+]Socket successfully binded..\n");
    }

    while (1)
    {
        if (1) // Now server is ready to listen and verification
        {
            // Now server is ready to listen and verification
            if ((listen(sockfd, 5)) != 0)
            {
                printf("[-] Listen failed...\n");
                exit(0);
            }
            else
                printf("[+] pop3 Server listening..\n");
            len = sizeof(cli);

            // Accept the data packet from client and verification
            connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
            char str[SIZE];

            if (connfd < 0)
            {
                printf("[-] pop3 server accept failed...\n");
                exit(0);
            }
            else
            {
                strcpy(str, "[+] 200 OK Connection is set up with POP3.");
                printf("%s\n", str);
                send(connfd, str, sizeof(str), 0);
            }
        }

        // Function for chatting between client and server
        char buff[SIZE], str[SIZE], fusername[30], fpassword[30];
        FILE *fp = fopen("userlogincred.txt", "r+");
        int count = 0, i = 0;
        while (fscanf(fp, "%[^ ]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        {
            count++;
        }
        struct user *Users;
        Users = calloc(sizeof(struct user), count);
        fseek(fp, 0, SEEK_SET);
        while (fscanf(fp, "%[^ ]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        {
            strcpy(Users[i].uname, fusername);
            strcpy(Users[i].passwd, fpassword);
            i++;
        }
        fclose(fp);

        while (1)
        {
            bzero(str, sizeof(str));
            recv(connfd, str, sizeof(str), 0); // get username and passwd

            if (strcmp(str, "3 EXIT.") == 0)
                break;
            // printf("**%s**\n", str);
            char str1[SIZE], str2[SIZE];
            split_msg(str, str1, str2);
            // printf("%s--%s\n", str1, str2);
            int flag = 0;
            for (int j = 0; j < count; j++)
            {
                char uname[30], password[30];
                strcpy(uname, Users[j].uname);
                strcpy(password, Users[j].passwd);
                if (strcmp(str1, uname) == 0)
                {
                    if (strcmp(str2, password) == 0)
                    {
                        flag = 1;
                        sprintf(buff, "305 User Authenticated with password\nWelcome %s to POP3 server\n", uname);
                        printf("[+] 305 User %s Authenticated with password\n", Users[j].uname);
                        // user.is_authenticated = 1;
                        // strcpy(user.uname, uname);
                        send(connfd, buff, sizeof(buff), 0);
                        acknowledge(connfd);
                        session_pop3(connfd, Users[j], Users, count); // commence session if user authenticatedfclose(fp);

                        fclose(fp);
                        break;
                    }
                    else
                    {
                        strcpy(buff, "300 Correct Username; WRONG password\nTry Again.\n");
                        send(connfd, buff, sizeof(buff), 0);
                        break;
                    }
                }
            }
            if (flag == 0)
            {
                strcpy(buff, "310 Incorrect Username\nTry agin.\n");
                send(connfd, buff, sizeof(buff), 0);
            }
            else
                break;
        }

        // printf("---------\n");

        close(connfd);
    }
    close(sockfd);
    return 0;
}
