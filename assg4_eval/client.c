

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

void *recMsg(void *addr)
{
	int addr1 = *((int *)addr);
	char buff[1024];
	int nb;
	bzero(buff, 1024);
	while (1)
	{
		nb = recv(addr1, buff, sizeof(buff), 0);
		if (nb > 0)
		{
			buff[nb] = '\0';
			printf("%s\n", buff);

			bzero(buff, 1024);
		}
	}
	return NULL;
}

int main()
{

	int clientSocket;
	char buffer[1024];
	socklen_t addr_size;
	struct sockaddr_in serverAddr;

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;

	serverAddr.sin_port = htons(7893);

	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof(serverAddr);

	connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);

	int *addr = &clientSocket;

	pthread_t tid;
	char msg[1024];
	pthread_create(&tid, NULL, recMsg, (void *)addr);
	bzero(buffer, 1024);
	bzero(msg, 1024);
	while (1)
	{
		scanf("%[^\n]%*c", buffer);
		bzero(msg, 1024);
		strcpy(msg, buffer);

		if (strncmp(msg, "Bye", 4) == 0)
		{			
			close(clientSocket);
			return 0;
		}
		send(clientSocket, msg, strlen(msg), 0);

		bzero(buffer, 1024);
		bzero(msg, 1024);
	}
	return 0;
}