#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX 80
#define SA struct sockaddr
#include <arpa/inet.h>
#include <time.h>
#define SIZE 500

int PORT;

void acknowledge(int sockfd)
{
	char ack[SIZE];
	bzero(ack, sizeof(ack));
	recv(sockfd, ack, sizeof(ack), 0);
	// if (strncmp(ack, "OK", 2) == 0)
	// {
	// 	printf("acknowledged\n");
	// }
	// else
	// 	printf("not acknowledged\n");
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

void sendmail(int sockfd, char uname[])
{
	char mail[SIZE], str[SIZE];
	bzero(mail, sizeof(mail));
	while (scanf("%[^\n]%*c", str))
	{
		strcat(mail, str);
		strcat(mail, "\n");
		if (strcmp(str, ".") == 0)
			break;
	}
	send(sockfd, mail, sizeof(mail), 0);
	acknowledge(sockfd);
	return;
}

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		PORT = atoi(argv[1]);
		printf("%d\n", PORT);
	}
	int sockfd, connfd;
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
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servaddr.sin_port = htons(PORT);

		char str[SIZE];
		// connect the client socket to server socket
		if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
		{
			printf("connection with the server failed...\n");
			exit(0);
		}
		else
		{
			recv(sockfd, str, sizeof(str), 0);
			printf("Server:\t %s\n", str);
		}
	}

	char str[SIZE], str1[SIZE], str2[SIZE], uname[30];
	printf("Enter Username and Password:\n");
	while (1) // LOGIN
	{
		bzero(str, sizeof(str));
		scanf("%[^\n]%*c", str);
		// printf("%s**\n", str);
		split_msg(str, str1, str2);
		strcpy(uname, str1);
		send(sockfd, str, sizeof(str), 0);
		bzero(str, sizeof(str));
		recv(sockfd, str, sizeof(str), 0);
		printf("---------------\n%s---------------\n", str);
		if (strncmp(str, "305", 3) == 0)
			break;
	}
	char msg[1000] = "1. Manage Mail: Shows the stored mails of the logged-in user only\n2. Send Mail: Allows the user to send a mail\n3. Quit: Quits the program.\n";
	printf("%s", msg);
	while (1)
	{
		bzero(str, sizeof(str));
		scanf("%[^\n]%*c", str);
		printf("**--%s--**\n", str);
		if (str[0] == '1') // Manage mail
		{
			printf("manage mail\n");
		}
		else if (str[0] == '2') // Send Mail.
		{
			printf("send mail\n");
			strcpy(str, "1 SendMail.");
			send(sockfd, str, sizeof(str), 0);
			acknowledge(sockfd);
			sendmail(sockfd, uname);
		}
		else if (str[0] == '3') // Exit
		{
			strcpy(str, "3 EXIT.");
			printf("exit\n");
			send(sockfd, str, sizeof(str), 0);
			acknowledge(sockfd);
			break;
		}
		// USERN ignatious
		// PASSWD 1234
		else
		{
			printf("Invalid input\n%s", msg);
		}
	}

	// close the socket
	close(sockfd);
	return 0;
}
