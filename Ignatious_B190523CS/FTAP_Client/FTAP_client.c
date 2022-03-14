#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#include <arpa/inet.h>
#include <time.h>
#define SIZE 500

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

int store_file(int connfd, char filename[])
{
	int n;
	char data[SIZE] = {0};

	bzero(data, SIZE);
	FILE *fp;
	fp = fopen(filename, "r");
	// printf("-%s-\n", filename);
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
		bzero(data, SIZE);
		
		int bs = fread(data, sizeof(char), 500, fp);
		// printf("#%d#\n", bs);
		if (bs == 0)
		{
			bzero(data, SIZE);
			// printf("**file send**\n");
			strcpy(data, "file_sent");
			send(connfd, data, sizeof(data), 0);
			acknowledge(connfd);
			break;
		}
		int x = send(connfd, data, bs, 0);
		// printf("%d --\n", x);
		acknowledge(connfd);
		// printf("**\n");
	}
	printf("--\nFile stored\n--\n");
	fclose(fp);
	return 0;
}

double get_file(int connfd, char filename[])
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
		if (strncmp(str, "file_sent", 5) == 0)
		{
			break;
		}
		if (strncmp(str, "error", 5) == 0)
		{
			return -1;
		}
		buffer[n] = '\0';

		fwrite(buffer, sizeof(char), n, fp);
		bzero(buffer, SIZE);
	}
	time_t end = time(NULL);
	double time_taken = difftime(end, start);
	fclose(fp);
	return time_taken;
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

int main()
{
	int sockfd, connfd;
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
	char str1[SIZE], str2[SIZE];
	printf("Enter Input:\n");
	while (1)
	{
		bzero(str, sizeof(str));
		scanf("%[^\n]%*c", str);
		// printf("%s**\n", str);
		split_msg(str, str1, str2);

		if (strcmp(str1, "StoreFile") == 0)
		{
			// printf("hello\n");
			send(sockfd, str, sizeof(str), 0);

			char ack[SIZE];
			bzero(ack, sizeof(ack));
			recv(sockfd, ack, sizeof(ack), 0);
			// printf("%s-- ", ack);

			int flag = store_file(sockfd, str2);
			
		}
		else if (strcmp(str1, "GetFile") == 0)
		{
			send(sockfd, str, sizeof(str), 0);
			double time_taken = get_file(sockfd, str2);
			bzero(str, sizeof(str));
			recv(sockfd, str, sizeof(str), 0);
			// printf("--\n%s--\n", str);
			if (time_taken >= 0)
				printf("-- File Received sucessfully\ttime taken = %.2f s --\n", time_taken);
		}
		else if (strncmp(str, "Bye", 3) == 0)
		{
			send(sockfd, str, sizeof(str), 0);
			break;
		}
		// USERN ignatious
		// PASSWD 1234
		else
		{
			send(sockfd, str, sizeof(str), 0);
			bzero(str, sizeof(str));
			recv(sockfd, str, sizeof(str), 0);
			printf("--\n%s--\n", str);
		}
	}

	// close the socket
	close(sockfd);
	return 0;
}
