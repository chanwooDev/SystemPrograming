#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define MAX_CLNT 2

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock[MAX_CLNT];
	int clnt_cnt = 0, i;

	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;
	char message[BUF_SIZE];
	int str_len;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error!");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind () error!");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error!");
	clnt_addr_size = sizeof(clnt_addr);
	while (clnt_cnt != MAX_CLNT)
	{
		clnt_sock[clnt_cnt] = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		if (clnt_sock[clnt_cnt] == -1)
			error_handling("accept() error");
		else
		{
			printf("Connected client %d\n", clnt_cnt);
			clnt_cnt++;
		}
		// while((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
		// 	write(clnt_sock, message, str_len);
		// close(clnt_sock);
	}

	close(serv_sock);

	if (clnt_cnt == MAX_CLNT)
	{
		sprintf(message, "%s", "ready");
		for (i = 0; i < MAX_CLNT; i++)
		{
			write(clnt_sock[i], message, sizeof("ready"));
		}
	}
	else
	{
		printf("error occured\n");
		return 0;
	}
	

	while (1)
	{
		if( (str_len = read(clnt_sock[0], message, BUF_SIZE)) != 0 ) //턴 순서대로
			write(clnt_sock[0], message, str_len);
		else if( (str_len = read(clnt_sock[1], message, BUF_SIZE)) != 0 )
			write(clnt_sock[1], message, str_len);
	}

	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
