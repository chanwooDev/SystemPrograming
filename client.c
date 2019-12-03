#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message);
void printmsg();
void choicemsg();

int sock;
char message[BUF_SIZE];
int str_len = 0;

int main(int argc, char *argv[])
{

	struct sockaddr_in serv_addr;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	else
	{
		puts("Connected......wait untill there's two players");
		while (1)
		{
			if ((str_len = read(sock, message, BUF_SIZE - 1)) != 0)
			{
				message[str_len] = 0;
				if (strcmp(message, "ready") == 0)
				{
					printf("Game start!\n");
					break;
				}
			}
			else
			{
				close(sock);
				return 0;
			}
		}
	}

	fputs("Input your name(Q to quit) : ", stdout);
	fgets(message, BUF_SIZE, stdin);
	if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
	{
		close(sock);
		return 0;
	}
	write(sock, message, strlen(message)); //send name

	while (1)
	{
		printmsg(); //my turn
		choicemsg();
	}

	close(sock);
	return 0;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void printmsg()
{
	while (1)
	{
		if ((str_len = read(sock, message, BUF_SIZE - 1)) != 0)
		{
			message[str_len] = 0;
			if(strcmp(message,"end") == 0)
			{
				break;
			}
			else if(strcmp(message,"quit") == 0)
			{
				exit(0);
			}
			printf("%s", message);
		}
		else
		{
			close(sock);
			return;
		}
	}
	return ;
}

void choicemsg()
{
	fputs("\nInput your choice : ", stdout);
	fgets(message, BUF_SIZE, stdin);
	printf("%s\n",message);
	write(sock, message, strlen(message)); //send
}