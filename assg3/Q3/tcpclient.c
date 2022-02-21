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

int n;

int count, c;

void write_file(int sockfd)
{
	int n;
	FILE *fp;
	FILE *fpgr;
	char *filename = "recv.avi";
	char *filename1 = "plot.txt";
	char buffer[SIZE];
	bzero(buffer, SIZE);
	fp = fopen(filename, "w");
	fpgr = fopen(filename1, "w");

	double k = 0.1;
	int m = 10;
	
	
	count = 0;
	c = 0;
	double t = 0.0;
	time_t start = time(NULL);
	time_t end;
	printf("%ld----\n\n", time(NULL));

	printf("time %f bytes %d \n\n", t, count);
	fprintf(fpgr, "%f\t%d\n", t, count);
	while (1)
	{
		bzero(buffer, SIZE);
		n = recv(sockfd, buffer, SIZE, 0);
		// printf("%d--", n);

		char str[10];
		strncpy(str, buffer + strlen(buffer) - 9, 9);
		if (strncmp(str, "file_sent", 5) == 0)
		{
			t += 0.1;
			printf("time %f bytes %d \n\n", t, count);

			fprintf(fpgr, "%f\t%d\n", t, count);

			printf("%ld----", time(NULL));
			printf("file sent successfully\n");
			return;
		}
		buffer[n] = '\0';

		fwrite(buffer, sizeof(char), n, fp);

		count += n;
		end = time(NULL);

		if (difftime(end, start) >= 0.1)
		{
			t += 0.1;
			printf("time %f bytes %d \n", t, count);
			fprintf(fpgr, "%f\t%d\n", t, count);
			start = time(NULL);
		}
		bzero(buffer, SIZE);
	}

	printf("%ld----", time(NULL));
	return;
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

	// connect the client socket to server socket
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	printf("Enetr Input:\n");
	char str[200];
	while (1)
	{
		bzero(str, sizeof(str));
		scanf("%[^\n]%*c", str);
		// printf("%s**\n", str);

		write(sockfd, str, sizeof(str));

		if (strcmp(str, "GivemeyourVideo") == 0)
		{
			printf("[+]TRING TO : Data written in the file successfully.\n");
			write_file(sockfd);
			printf("[+]Data written in the file successfully.\n");
		}
		else if (strncmp(str, "Bye", 3) == 0)
		{
			break;
		}
		else
		{
			bzero(str, sizeof(str));
			read(sockfd, str, sizeof(str));
			printf("From Server:\t%s\n", str);
		}
	}

	// close the socket
	close(sockfd);
	return 0;
}
