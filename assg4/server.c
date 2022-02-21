

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

struct clientData
{
	int sd;
	char Name[20];
};

int count;
struct clientData arr[10];

void brodcast(struct clientData *p, char *buff, int fg)
{
	if (fg == 1)
	{

		strcat(buff, " has joined");
		for (int i = 0; i < count; i++)
		{
			if (arr[i].sd == p->sd || (arr[i].sd == -20))
			{
				continue;
			}
			// printf("%s  %d\n", buff, i);
			send(arr[i].sd, buff, strlen(buff), 0);
		}
	}
	else if (fg == 0)
	{
		char fmsg[1024];
		strcat(fmsg, p->Name);
		strcat(fmsg, " : ");
		strcat(fmsg, buff);
		for (int i = 0; i < count; i++)
		{
			if (arr[i].sd == p->sd || (arr[i].sd == -20))
			{
				continue;
			}
			// printf("22 %s  %d\n", fmsg, i);
			send(arr[i].sd, fmsg, strlen(fmsg), 0);
		}
		bzero(fmsg, 1024);
	}
	else
	{
		strcat(buff, " left the chat room");
		for (int i = 0; i < count; i++)
		{
			if (arr[i].sd == p->sd || (arr[i].sd == -20))
			{
				continue;
			}
			// printf("%s  %d\n", buff, i);
			send(arr[i].sd, buff, strlen(buff), 0);
		}

		close(p->sd);

		p->sd = -20;
	}
}

void *newClient(void *p1)
{

	char buff[1024];
	int nb;
	struct clientData *p = (struct clientData *)p1;
	nb = recv(p->sd, buff, sizeof(buff), 0);

	buff[nb] = '\0';

	strcpy(p->Name, buff);
	// printf("%s\n", buff)
	brodcast(p, buff, 1);
	printf("%s has connected\n", p->Name);
	bzero(buff, 1024);
	while (1)
	{
		if (p->sd != -20)
		{
			nb = recv(p->sd, buff, 1024, 0);
			buff[nb] = '\0';
			if (nb == 0)
			{
				bzero(buff, 1024);
				strcat(buff, p->Name);
				brodcast(p, buff, 2);
				printf("%s left the sever\n", p->Name);
			}
			else
			{
				brodcast(p, buff, 0);
				bzero(buff, 1024);
			}
		}
	}
}
int main()
{
	int welcomeSocket, newSocket;

	char buffer[1024];
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	socklen_t addr_size;

	count = 0;

	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7893);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	bind(welcomeSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

	if (listen(welcomeSocket, 10) == 0)
		printf("server is live\n");
	else
		printf("Error\n");

	int i = 0;
	while (1)
	{

		char buff[1024];
		addr_size = sizeof clientAddr;

		newSocket = accept(welcomeSocket, (struct sockaddr *)&clientAddr, &addr_size);

		arr[count].sd = newSocket;
		count++;

		pthread_t tid;

		pthread_create(&tid, NULL, newClient, arr + i);
		i++;
	}
	return 0;
}