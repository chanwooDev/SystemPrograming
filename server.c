/*
이 코드는 서버에서 돌아간다.
클라이언트는 전송받은 카드리스트에서 카드 선택한 정보 서버로 전송

구현 기능

 - 입력을 통해 상대방의 카드를 선택해서 가져올 수 있다.
 - 소켓을 통해 카드를 선택하는 정보를 전송하여 게임을 진행한다.
 - 타이머를 통해 특정시간 내에 상대방의 카드를 고르지 않을 경우, 자동으로 랜덤 선택하도록 한다.
 - 특정 시그널 키를 누를 경우 상대방에게 감정표현을 전송할 수 있다.

서버
   통신 및 하위 코드
클라이언트
   lcurses 통한 승리/패배 출력(감정 표현 시그널 통해서 출력)
   lcurses 전송 받은 덱 출력
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 1024
#define MAX_CLNT 2

typedef struct card {
   int num;
   char shape; //s d h c
}card;

typedef struct player {
   card deck[28];
   char name[20];
   int cardNum;   //가진 카드 개수
}player;

void set_name();
void makeDeck(card deck[]);
void suffle(card deck[], int num);
void pushDeck(card deck[], player* player1, player* player2);
void printDeck(player* p,int clientnum);
void arrangement(player *p,int clientnum);
int isClear(player* p);
void pushCard(player* p, card cardData);
void playerTurn(player* cur, player* opp, int choice);
void displayTo(player* p, int oppoCardNum,int clientnum);
void sendplayer(int clientnum, char* msg);
void ifgiveup(int clientnum,char * message);
void catching_thief();
void error_handling(char *message);

player player1, player2;
int clnt_sock[MAX_CLNT];
int clnt_cnt = 0;
int str_len;
char message[BUF_SIZE];

int main(int argc, char *argv[])
{
	int serv_sock;
	int i;

	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;

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


 	catching_thief();


	return 0;
}



void makeDeck(card deck[]) {
   for (int i = 1; i <= 4; i++) {
      for (int j = 13*(i-1)+1; j <= 13*(i-1)+ 13; j++) {
         deck[j].num = j - 13*(i-1);   
         switch (i) {
         case 1: 
            deck[j].shape = 's';         //스페이드 1 다이아 2 하트 3 클로버 4
            break;
         case 2:
            deck[j].shape = 'd';         //스페이드 1 다이아 2 하트 3 클로버 4
            break;
         case 3:
            deck[j].shape = 'h';         //스페이드 1 다이아 2 하트 3 클로버 4
            break;
         case 4:
            deck[j].shape = 'c';         //스페이드 1 다이아 2 하트 3 클로버 4
            break;
         }
      }
   }
   deck[0].num = 0;
   deck[0].shape = 'j';
}

void suffle(card deck[], int num) {
   srand(time(NULL));
   card temp;
   int rn;
   for (int i = 0; i < num - 1; i++) {
      rn = rand() % (num - i) + i;
      temp = deck[i];
      deck[i] = deck[rn];
      deck[rn] = temp;
   }
}

void pushDeck(card deck[], player* player1, player* player2) {
   for (int i = 0; i < 27; i++) {
      player1->deck[i] = deck[i];
   }
   player1->cardNum = 27;
   for (int i = 0; i < 26; i++) {
      player2->deck[i] = deck[i+27];
   }
   player2->cardNum = 26;
}

void printDeck(player* p,int clientnum) {
   sprintf(message,"\nyour current Deck:\n");
   sendplayer(clientnum,message);
   for (int i = 0; i < p->cardNum; i++) {
     sprintf(message,"%d %c ", p->deck[i].num, p->deck[i].shape);
	 sendplayer(clientnum,message);
   }
   sprintf(message,"\n---------------------------\n");
   sendplayer(clientnum,message);

}

void arrangement(player *p,int clientnum) {
   int i, j;
   card temp;
   int n = p->cardNum;
   //버블sort
   for (i = n - 1; i > 0; i--) {
      for (j = 0; j < i; j++) {
         if (p->deck[j].num < p->deck[j + 1].num) {
            temp= p->deck[j];
            p->deck[j] = p->deck[j + 1];
            p->deck[j + 1] = temp;
         }
      }
   }
   //discards
   
   for (int i = 0; i < p->cardNum-1; i++) {
      if (p->deck[i].num == p->deck[i+1].num)
      {
         sprintf(message, "%d %c,%d %c is discarded\n", p->deck[i].num, p->deck[i].shape, p->deck[i + 1].num, p->deck[i + 1].shape);
         sendplayer(clientnum, message);
         for (int j = i + 2; j < p->cardNum; j++)
         {
            p->deck[j - 2] = p->deck[j];
         }
         p->cardNum = p->cardNum - 2;
         i--;         //커서는 제자리로
      }

   }
   if(p->cardNum != 0)
      printDeck(p,clientnum);
}

int isClear(player* p) {
   if (p->cardNum) {
      return 0;
   }
   return 1;
}

void pushCard(player* p, card cardData) {
   p->deck[p->cardNum++] = cardData;
}

void playerTurn(player* cur, player* opp, int choice) {      //cur 현재, opponent 상대방
   card cardData = opp->deck[choice];
   for (int j = choice + 1; j < opp->cardNum; j++) {
      opp->deck[j - 1] = opp->deck[j];
   }
   opp->cardNum = opp->cardNum - 1;
   sprintf(message,"%d %c has chosen!\n", cardData.num, cardData.shape);
   sendplayer(0,message);
   sendplayer(1,message);
   pushCard(cur, cardData);
}
////////////// display/////////////////////
void displayTo(player* p, int oppoCardNum,int clientnum) {
	sprintf(message, "\n--------------%s display--------------",p->name);
	write(clnt_sock[clientnum], message, strlen(message));
	sprintf(message, "\ninput card number 0 ~ %d\n",oppoCardNum-1);
	write(clnt_sock[clientnum], message, strlen(message));
}

void sendplayer(int clientnum, char* msg)
{
	write(clnt_sock[clientnum], msg, strlen(msg));
}

void set_name(player* p,int clientnum)
{
	
	if ((str_len = read(clnt_sock[clientnum], message, sizeof(player1.name))) != 0) //턴 순서대로
	{
		message[str_len-1] = '\0'; //개행
		strcpy(p->name,message);
      sprintf(message,"Welcome %s\n",p->name);
      sendplayer(clientnum, message);
	}
	else
	{
		printf("socket error");
		exit(1);
	}
	
	return;
}

int getFrom(int clientnum){
   if ((str_len = read(clnt_sock[clientnum], message, sizeof(message))) != 0)
	{
		message[str_len] = '\0';
	}
	else
	{
		printf("socket error");
		exit(1);
	}
   ifgiveup(clientnum,message);

	return atoi(message);
}

void ifgiveup(int clientnum, char* message)
{
   if (strcmp(message, "giveup") == 0)
   {
      if(clientnum == 0)
      {
         player2.cardNum = 0;
      }
      else
      {
         player1.cardNum = 0;
      }
   }
}

void catching_thief() {
   card deck[53]; //전체 덱
   card cardData; //상대한테 받은 카드 하나 (통신)
   cardData.num = 0;
   cardData.shape = 0;
   int choice=0;
  

   set_name(&player1,0); //통신
   set_name(&player2,1);
   makeDeck(deck);
   suffle(deck, 53);         //전체 카드 53개
   pushDeck(deck, &player1, &player2); //덱을 나눠줌
   arrangement(&player1,0);      //소팅도 하고 뺄수 있는 패 정리 
   arrangement(&player2,1);


   
   while (!isClear(&player1) && !isClear(&player2)) { //isClear 함수를 통해서 덱이 있냐 없냐
      suffle(player2.deck, player2.cardNum);         //플2 카드를 섞음
      displayTo(&player1,player2.cardNum,0);         //player1 에게 화면 출력
      sleep(1); //to send end..
      sendplayer(0, "end");
      choice = getFrom(0);/* 통신 > 넘버 받아옴 (변수 choice)*/
      if (isClear(&player2))            //끝났는지 체크
         break;
      playerTurn(&player1, &player2, choice); //플2 덱에서 플1이 카드선택후 리턴//이 함수 안에서 통신사용 or 숫자만 가져오기
      arrangement(&player1,0);                  //시간복잡도 고려해서 수정할 필요있음
      if (isClear(&player1))            //끝났는지 체크
         break;

      suffle(player1.deck, player1.cardNum);         //플1 카드를 섞음
      displayTo(&player2, player1.cardNum,1);         //player2 에게 화면 출력
      sleep(1);
      sendplayer(1, "end");
      choice = getFrom(1);/* 통신 > 넘버 받아옴 (변수 choice)*/
      if (isClear(&player1))            //끝났는지 체크
         break;
      playerTurn(&player2, &player1, choice);
      arrangement(&player2,1);
   }
   
   if (isClear(&player1))
   {
      sprintf(message,"\n*****************************************\n");
      sendplayer(0, message);
      sendplayer(1, message);
      sprintf(message,"*************player1 >>%s win*************\n", player1.name);
      sendplayer(0, message);
      sendplayer(1, message);
      sprintf(message,"*****************************************\n");
      sendplayer(0, message);
      sendplayer(1, message);
      sleep(1);
      sendplayer(0, "quit");
      sendplayer(1, "quit");
   }
   else
   {
      sprintf(message,"\n*****************************************\n");
      sendplayer(0, message);
      sendplayer(1, message);
      sprintf(message,"************player2 >>%s<< win************\n", player2.name);
      sendplayer(0, message);
      sendplayer(1, message);
      sprintf(message,"*****************************************\n");
      sendplayer(0, message);
      sendplayer(1, message);
      sleep(1);
      sendplayer(0, "quit");
      sendplayer(1, "quit");
   }
   

   return ;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
