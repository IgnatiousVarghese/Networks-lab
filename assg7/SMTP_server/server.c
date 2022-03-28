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
void get_time_string(char *time_string){
    time_t current_time;
    struct tm *timeinfo;

    current_time = time(NULL);
    timeinfo = localtime(&current_time);
    strftime(time_string, 100, "%a, %d %b %Y %I:%M:%S Localtime", timeinfo);
}


void session(int sockfd, struct user User, struct user *Users, int count)
{
	char str[SIZE], mail[XSIZE];
	while (1)
	{
		bzero(str, sizeof(str));
		recv(sockfd, str, sizeof(str), 0);
		char ack[SIZE];
		strcpy(ack, "session option received.\n");
		send(sockfd, ack, sizeof(ack), 0);
		printf("%s\n", str);
		if (strcmp(str, "2 SendMail.") == 0)
		{
			bzero(mail, sizeof(mail));
			recv(sockfd, mail, sizeof(mail), 0);
			printf("[+] Received mail.\n");
			// printf("-----\n%s-----\n", mail);

			// acknowledge received mail.
			// strcpy(ack, "mail received ack.\n");
			// send(sockfd, ack, sizeof(ack), 0);

			// validate received mail.
			char from[100], to[100], subject[100], body[SIZE];

			if (get_field(mail, "From", from) == -1 || !verify_email(from))
			{
				strcpy(str, "Invalid From field.\n");
				printf("[-] %s", str);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}
			if (get_field(mail, "To", to) == -1 || !verify_email(to))
			{
				strcpy(str, "Invalid To field.\n");
				printf("[-] %s", str);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}
			if (get_field(mail, "Subject", subject) == -1)
			{
				strcpy(str, "Invalid Subject field.\n");
				printf("[-] %s", str);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}
			if (get_field(mail, "Body", body) == -1)
			{
				strcpy(str, "Invalid Body field.\n");
				printf("[-] %s", str);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}

			// printf("from:*%s*\nto:*%s*\nsub:*%s*\nbody:*%s*\n", from, to, subject, body);

			// verify if "from" email is logged in user.
			if (strncmp(User.uname, from, strlen(User.uname)) != 0)
			{
				strcpy(str, "From address not of logged in user's\n");
				printf("[-] %s :--%s--\n", str, User.uname);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}
			// verify if "to" email is present.
			int flag = 0;
			struct user reciever;
			for(int i = 0; i<count; i++)
			{
				if (strncmp(Users[i].uname, to, strlen(Users[i].uname)) == 0)
				{
					flag = 1;
					reciever = Users[i];
					break;
				}
			}
			if(flag == 0)
			{
				strcpy(str, "Receiver addres Not present.\n");
				printf("[-] %s", str);
				send(sockfd, str, sizeof(str), 0);
				continue;
			}

			//verified
			// enter mail to mailbox file of receiver.

			char mailbox_fname[100] = {0};
			sprintf(mailbox_fname, "%s/mymailbox.mail", reciever.uname);
			printf("filename : --%s--\n", mailbox_fname);
			FILE *mailbox = fopen(mailbox_fname, "a");

			char time_str[50];
			get_time_string(time_str);
			printf("time:--%s--\n", time_str);

			fprintf(mailbox, "From: %s\n", from);
			fprintf(mailbox, "To: %s\n", to);
			fprintf(mailbox, "Subject: %s\n", subject);
			fprintf(mailbox, "Received: %s\n", time_str);
			fprintf(mailbox, "%s", body);

			fclose(mailbox);
			
			strcpy(str, "EMAIL SENT\n");
			printf("[+] %s has sent mail to %s.\n", from, to);
			send(sockfd, str, sizeof(str), 0);
		}
		else if (strcmp(str, "3 EXIT.") == 0)
			break;
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
	}

	// Function for chatting between client and server
	char buff[SIZE], str[SIZE], fusername[30], fpassword[30];
	FILE *fp = fopen("userlogincred.txt", "r");
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
		// printf("--%s--%s--\n", Users[i].uname, Users[i].passwd);
		i++;
	}
	while (1)
	{
		bzero(str, sizeof(str));
		recv(connfd, str, sizeof(str), 0); // get username and passwd
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
				flag = 1;
				if (strcmp(str2, password) == 0)
				{
					sprintf(buff, "305 User Authenticated with password\nWelcome %s\n", uname);
					printf("[+] 305 User %s Authenticated with password\n", uname);
					// user.is_authenticated = 1;
					// strcpy(user.uname, uname);
					send(connfd, buff, sizeof(buff), 0);
					session(connfd, Users[j], Users, count); // commence session if user authenticatedfclose(fp);

					fclose(fp);
					close(connfd);
					close(sockfd);
					return 0;
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
	}
	fclose(fp);

	// printf("---------\n");

	close(connfd);
	close(sockfd);
	return 0;
}
