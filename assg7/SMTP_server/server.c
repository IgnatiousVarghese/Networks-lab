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

void session(int sockfd, struct user User)
{
	char str[SIZE], mail[SIZE];
	while (1)
	{
		bzero(str, sizeof(str));
		recv(sockfd, str, sizeof(str), 0);
		char ack[SIZE];
		strcpy(ack, "session work received.\n");
		send(sockfd, ack, sizeof(ack), 0);
		printf("%s\n", str);
		if (strcmp(str, "1 SendMail.") == 0)
		{
			bzero(mail, sizeof(mail));
			recv(sockfd, mail, sizeof(mail), 0);
			printf("-----\n%s-----\n", mail);
			// validate received mail. 
			strcpy(ack, "mail received ack.\n");
			send(sockfd, ack, sizeof(ack), 0);
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
		printf("--%s--%s--\n", fusername, fpassword);
		strcpy(Users[i].uname, fusername);
		strcpy(Users[i].passwd, fpassword);
		printf("--%s--%s--\n", Users[i].uname, Users[i].passwd);
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
					session(connfd, Users[j]); // commence session if user authenticatedfclose(fp);

					fclose(fp);
					close(connfd);
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
	return 0;
}
