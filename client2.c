#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <curses.h>
#include <termios.h>
#include <signal.h>

#define BUF_SIZE 1024

void error_handling(char *message);
void printmsg();
void choicemsg();
void giveup(int signum);

int sock;
char message[BUF_SIZE];
int str_len = 0;

int i = 0;

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

	initscr();
	clear();
	move(0, 0);
	addstr("Game start!");
	move(1, 0);
	addstr("Input your name(Q to quit) : ");
	refresh();
	getstr(message);
	clear();
	if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
	{
		close(sock);
		return 0;
	}
	write(sock, message, strlen(message)); //send name
	signal(SIGINT, giveup);
	while (1)
	{
		
		printmsg(); //my turn
		//clear();
	//	choicemsg();
		//	clear();
	}
	

}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void printmsg()
{
	int count = 0;
	while (1)
	{
		if (count)
		{
			refresh();
    	}

		if ((str_len = read(sock, message, BUF_SIZE - 1)) != 0)
		{
			message[str_len] = 0;
			if (strcmp(message, "end") == 0)
			{
				break;
			}
			else if (strcmp(message, "quit") == 0)
			{	
				sleep(3);
				clear();
				addstr(message);
				close(sock);
				endwin();
				exit(1);
			}
		//	move(i++, 0);
			
			addstr(message);
		//	move(count, 0);
			refresh();

		}
		else
		{
			close(sock);
			return;
		}
		count++;
	}

	move(20, 0);
	addstr("Input your choice :");
	refresh();
	getstr(message);
	write(sock, message, strlen(message)); //send
	clear();
	return;
}


void giveup(int signum)
{
	write(sock, "giveups", strlen("giveup")); //send
	clear();
	addstr("\n*****************************************\n*************You give up!*************\n*****************************************\n");
	refresh();
	sleep(3);
	close(sock);
	endwin();
	exit(0);
	
}







/*void choicemsg()
{
	move(20, 0); 
	addstr("Input your choice :");
	refresh();
	getstr(message);
	write(sock, message, strlen(message)); //send
	clear();
}*/


