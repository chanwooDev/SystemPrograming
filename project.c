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
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>

typedef struct card {
	int num;
	char shape; //s d h c
}card;

typedef struct player {
	card deck[28];
	char name[20];
	int cardNum;   //가진 카드 개수
}player;



void makeDeck(card deck[]) {
	for (int i = 1; i <= 4; i++) {
		for (int j = 13 * (i - 1) + 1; j <= 13 * (i - 1) + 13; j++) {
			deck[j].num = j - 13 * (i - 1);
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
		player2->deck[i] = deck[i + 27];
	}
	player2->cardNum = 26;
}
void printDeck(player* p) {
	printf("\nyour current Deck:\n");
	for (int i = 0; i < p->cardNum; i++) {
		printf("%d %c ", p->deck[i].num, p->deck[i].shape);
	}
	printf("\n---------------------------\n");

}
void arrangement(player* p) {
	int i, j;
	card temp;
	int n = p->cardNum;
	//버블sort
	for (i = n - 1; i > 0; i--) {
		for (j = 0; j < i; j++) {
			if (p->deck[j].num < p->deck[j + 1].num) {
				temp = p->deck[j];
				p->deck[j] = p->deck[j + 1];
				p->deck[j + 1] = temp;
			}
		}
	}
	//discards

	for (int i = 0; i < p->cardNum - 1; i++) {
		if (p->deck[i].num == p->deck[i + 1].num)
		{
			printf("%d %c,%d %c is discarded\n", p->deck[i].num, p->deck[i].shape, p->deck[i + 1].num, p->deck[i + 1].shape);
			for (int j = i + 2; j < p->cardNum; j++) {
				p->deck[j - 2] = p->deck[j];
			}
			p->cardNum = p->cardNum - 2;
			i--;         //커서는 제자리로
		}

	}
	if (p->cardNum != 0)
		printDeck(p);
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
	printf("%d %c 가 뽑혔습니다\n", cardData.num, cardData.shape);
	pushCard(cur, cardData);
}
////////////// display/////////////////////
void displayTo(player* p, int oppoCardNum) {
	printf("\n--------------%s display--------\n", p->name);
	printf("input card number 0 ~ %d\n", oppoCardNum - 1);

}


void catching_thief() {
	card deck[53]; //전체 덱
	player player1, player2;
	card cardData; //상대한테 받은 카드 하나 (통신)
	cardData.num = 0;
	cardData.shape = 0;
	int choice = 0;
	printf("player1 :");
	scanf("%s", player1.name);//player 이름 설정 함수
	printf("player2 :");
	scanf("%s", player2.name);//player 이름 설정 함수
	makeDeck(deck);
	suffle(deck, 53);         //전체 카드 53개
	pushDeck(deck, &player1, &player2); //덱을 나눠줌
	arrangement(&player1);      //소팅도 하고 뺄수 있는 패 정리 
	arrangement(&player2);



	//통신 준비완료 후
	while (!isClear(&player1) && !isClear(&player2)) { //isClear 함수를 통해서 덱이 있냐 없냐
		suffle(player2.deck, player2.cardNum);         //플2 카드를 섞음
		displayTo(&player1, player2.cardNum);         //player1 에게 화면 출력
		scanf("%d", &choice);/* 통신 > 넘버 받아옴 (변수 choice)*/
		playerTurn(&player1, &player2, choice); //플2 덱에서 플1이 카드선택후 리턴//이 함수 안에서 통신사용 or 숫자만 가져오기
		arrangement(&player1);                  //시간복잡도 고려해서 수정할 필요있음
		if (isClear(&player1))            //끝났는지 체크
			break;

		suffle(player1.deck, player1.cardNum);         //플1 카드를 섞음
		displayTo(&player2, player1.cardNum);         //player2 에게 화면 출력
		scanf("%d", &choice);/* 통신 > 넘버 받아옴 (변수 choice)*/
		playerTurn(&player2, &player1, choice);
		arrangement(&player2);
	}

	if (isClear(&player1))
	{
		printf("\n*****************************************\n");
		printf("*************player1 >>%s win************\n", player1.name);
		printf("*****************************************\n");
		//display_win_player1();      //player1에게 승리 화면
		//display_lose_player2();      //player2 에게 패배 화면
		//changeRecord();            //기록 수정 - file IO
	}
	else
	{
		printf("\n*****************************************\n");
		printf("*********   player2 >>%s<< win************\n", player2.name);
		printf("*****************************************\n");
		//display_win_player2();      //player1에게 승리 화면
		//display_lose_player1();      //player2 에게 패배 화면
		//changeRecord();            //기록 수정 - file IO
	}


	return 0;
}
int main() {

	catching_thief();
	return 0;
}