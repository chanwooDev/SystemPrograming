/*
�� �ڵ�� �������� ���ư���.
Ŭ���̾�Ʈ�� ���۹��� ī�帮��Ʈ���� ī�� ������ ���� ������ ����

���� ���

 - �Է��� ���� ������ ī�带 �����ؼ� ������ �� �ִ�.
 - ������ ���� ī�带 �����ϴ� ������ �����Ͽ� ������ �����Ѵ�.
 - Ÿ�̸Ӹ� ���� Ư���ð� ���� ������ ī�带 ���� ���� ���, �ڵ����� ���� �����ϵ��� �Ѵ�.
 - Ư�� �ñ׳� Ű�� ���� ��� ���濡�� ����ǥ���� ������ �� �ִ�.

����
   ��� �� ���� �ڵ�
Ŭ���̾�Ʈ
   lcurses ���� �¸�/�й� ���(���� ǥ�� �ñ׳� ���ؼ� ���)
   lcurses ���� ���� �� ���
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
	int cardNum;   //���� ī�� ����
}player;



void makeDeck(card deck[]) {
	for (int i = 1; i <= 4; i++) {
		for (int j = 13 * (i - 1) + 1; j <= 13 * (i - 1) + 13; j++) {
			deck[j].num = j - 13 * (i - 1);
			switch (i) {
			case 1:
				deck[j].shape = 's';         //�����̵� 1 ���̾� 2 ��Ʈ 3 Ŭ�ι� 4
				break;
			case 2:
				deck[j].shape = 'd';         //�����̵� 1 ���̾� 2 ��Ʈ 3 Ŭ�ι� 4
				break;
			case 3:
				deck[j].shape = 'h';         //�����̵� 1 ���̾� 2 ��Ʈ 3 Ŭ�ι� 4
				break;
			case 4:
				deck[j].shape = 'c';         //�����̵� 1 ���̾� 2 ��Ʈ 3 Ŭ�ι� 4
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
	//����sort
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
			i--;         //Ŀ���� ���ڸ���
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
void playerTurn(player* cur, player* opp, int choice) {      //cur ����, opponent ����
	card cardData = opp->deck[choice];
	for (int j = choice + 1; j < opp->cardNum; j++) {
		opp->deck[j - 1] = opp->deck[j];
	}
	opp->cardNum = opp->cardNum - 1;
	printf("%d %c �� �������ϴ�\n", cardData.num, cardData.shape);
	pushCard(cur, cardData);
}
////////////// display/////////////////////
void displayTo(player* p, int oppoCardNum) {
	printf("\n--------------%s display--------\n", p->name);
	printf("input card number 0 ~ %d\n", oppoCardNum - 1);

}


void catching_thief() {
	card deck[53]; //��ü ��
	player player1, player2;
	card cardData; //������� ���� ī�� �ϳ� (���)
	cardData.num = 0;
	cardData.shape = 0;
	int choice = 0;
	printf("player1 :");
	scanf("%s", player1.name);//player �̸� ���� �Լ�
	printf("player2 :");
	scanf("%s", player2.name);//player �̸� ���� �Լ�
	makeDeck(deck);
	suffle(deck, 53);         //��ü ī�� 53��
	pushDeck(deck, &player1, &player2); //���� ������
	arrangement(&player1);      //���õ� �ϰ� ���� �ִ� �� ���� 
	arrangement(&player2);



	//��� �غ�Ϸ� ��
	while (!isClear(&player1) && !isClear(&player2)) { //isClear �Լ��� ���ؼ� ���� �ֳ� ����
		suffle(player2.deck, player2.cardNum);         //��2 ī�带 ����
		displayTo(&player1, player2.cardNum);         //player1 ���� ȭ�� ���
		scanf("%d", &choice);/* ��� > �ѹ� �޾ƿ� (���� choice)*/
		playerTurn(&player1, &player2, choice); //��2 ������ ��1�� ī�弱���� ����//�� �Լ� �ȿ��� ��Ż�� or ���ڸ� ��������
		arrangement(&player1);                  //�ð����⵵ ����ؼ� ������ �ʿ�����
		if (isClear(&player1))            //�������� üũ
			break;

		suffle(player1.deck, player1.cardNum);         //��1 ī�带 ����
		displayTo(&player2, player1.cardNum);         //player2 ���� ȭ�� ���
		scanf("%d", &choice);/* ��� > �ѹ� �޾ƿ� (���� choice)*/
		playerTurn(&player2, &player1, choice);
		arrangement(&player2);
	}

	if (isClear(&player1))
	{
		printf("\n*****************************************\n");
		printf("*************player1 >>%s win************\n", player1.name);
		printf("*****************************************\n");
		//display_win_player1();      //player1���� �¸� ȭ��
		//display_lose_player2();      //player2 ���� �й� ȭ��
		//changeRecord();            //��� ���� - file IO
	}
	else
	{
		printf("\n*****************************************\n");
		printf("*********   player2 >>%s<< win************\n", player2.name);
		printf("*****************************************\n");
		//display_win_player2();      //player1���� �¸� ȭ��
		//display_lose_player1();      //player2 ���� �й� ȭ��
		//changeRecord();            //��� ���� - file IO
	}


	return 0;
}
int main() {

	catching_thief();
	return 0;
}