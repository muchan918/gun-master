#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <process.h>
#pragma comment (lib,"winmm.lib");

// ���� ����
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define PLAYER "��"
#define GUN "��"
#define ZGUN "��"
#define LaserR "��"
#define LaserC "��"
#define Zombie "��"
#define BLANK '  ' // ' ' ���ϸ� ������ �������� 

#define ESC 0x1b //  ESC ������ ����

#define SPECIAL1 0xe0 // Ư��Ű�� 0xe0 + key ������ �����ȴ�.
#define SPECIAL2 0x00 // keypad ��� 0x00 + key �� �����ȴ�.

#define UP  0x48 // Up key�� 0xe0 + 0x48 �ΰ��� ���� ���´�.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define UP2		'w'//�ѽ��� AWSD �� ����Ű ���
#define DOWN2	's'
#define LEFT2	'a'
#define RIGHT2	'd'

#define WIDTH 80
#define HEIGHT 24
#define MAX_B = 100;


int Delay = 10; // 100 msec delay, �� ���� ���̸� �ӵ��� ��������.
int keep_moving = 1; // 1:����̵�, 0:��ĭ���̵�.
int called[2];
int life = 3;
int score = 0;
int restrict_zombie = 1;// ���� ���� �� �ð� ������ �þ
int weapon[60][HEIGHT] = { 0 }; // �ʿ� ���� �������� �������� ����
int zombie[60][HEIGHT] = { 0 }; // �ʿ� ���� �������� ����
int weapon_kind;
int weapon_num; // �ʿ� ������ ������ ��(2�������� ����)
int b = 0;
int c = 0;
int d = 0;
int e = 0;
int check_e;

int stage = 0; // ���̵� ����
int stage2 = 0;

int zombie_1_P;
int zombie_2_P;
int bullet[5] = { 0 }; // �� ������ ����(0~4) �Ѿ� ���� �ٸ���

int zombie_count = 0; // ���� ��
int zombie_appear = 50;

int frame_count = 0;
int bullet_frame_sync = 2; //�Ѿ� ������ �ӵ�
int zombie1_frame_sync = 50; // ���� 1
int zombie2_frame_sync = 10; // ���� 2
int zombie3_frame_sync = 40; // ���� 3
int weapon_frame_sync = 100; // ���� ������ ����
//��������
/*weapon
0 : ����(P) : �⺻
1 : �����(M) : �Ѿ� �ӵ� ������, ���� �Ȱ��� �� �� �ִ�?
2 : ����(S) : �� �Ʒ� 3�߾� ����
3 : ������(L) : �ӵ� �̳� ������ �ϰ� �Ѿ��� ���������� �����
4 : ��ź(B) : ���ҵ�
*/

//��������
/* zombie
1 : �Ϲ� ����
2 : ���� ��� ����
3 : �ӵ��� ���� ����
*/
//������ �ڵ�
void removeCursor(void) { // Ŀ���� �Ⱥ��̰� �Ѵ�

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //���� ���ϴ� ��ġ�� Ŀ�� �̵�
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API �Լ��Դϴ�. �̰� ���ʿ� �����
}

void putstar(int x, int y, char* ch)
{
	gotoxy(x, y);
	printf("%s", ch);
}
void erasestar(int x, int y)
{
	gotoxy(x, y);
	putchar(BLANK);
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}
// ȭ�� ������ ���ϴ� �������� �����Ѵ�.
void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);

}
void draw_box2(int x1, int y1, int x2, int y2, char* ch)
{
	int x, y;
	// �ϼ��� �� ������ �� ��
	for (x = x1; x <= x2; x += 2) {
		gotoxy(x, y1);
		printf("%s", ch);
		gotoxy(x, y2);
		printf("%s", ch);
	}
	for (y = y1; y <= y2; y += 1) {
		gotoxy(x1, y);
		printf("%s", ch);
		gotoxy(x2, y);
		printf("%s", ch);
	}
}
//���� �׸���
void draw_col(int x, char* ch) {
	int i;
	for (i = 0; i <= HEIGHT; i++) {
		gotoxy(x, i);
		printf("%s", ch);
	}
}
//���ڷ� �ڽ� �׸���
void draw_box2_move(int x1, int y1, int x2, int y2, char* ch) {
	int i = rand() % 6 + 1;
	textcolor(i, WHITE);
	draw_box2(x1, y1, x2, y2, ch);
	Sleep(300);
}
//������ �ڽ� �׸���
void draw_box_line(int x1, int y1, int x2, int y2)
{
	int x, y;
	//int len = strlen(ch);
	for (x = x1; x <= x2; x += 2) { // �ѱ��� 2ĭ�� �����Ѵ�.
		gotoxy(x, y1);
		printf("%s", "��"); // �� ������ ����Ű ������ ����
		gotoxy(x, y2);
		printf("%s", "��");
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%s", "��");
		gotoxy(x2, y);
		printf("%s", "��");
	}
	gotoxy(x1, y1); printf("��");
	gotoxy(x1, y2); printf("��");
	gotoxy(x2, y1); printf("��");
	gotoxy(x2, y2); printf("��");
}
//���� ���ο� ���� �Լ���
void game_main_name()
{
	int game_name_Gun[6][18] = {
		{0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,0,0},
		{1,0,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0},
		{1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0},
		{0,1,1,1,1,0,0,0,1,1,0,0,0,1,0,0,1,0}
	};
	int game_name_Mas[6][18] = {
		{1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0},
		{1,0,1,1,0,1,0,1,0,0,1,0,0,1,0,0,0,0},
		{1,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0},
		{1,0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1,0},
		{1,0,0,0,0,1,0,0,1,1,0,0,1,0,1,1,0,0}
	};
	int game_name_ter[6][18] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0},
		{0,1,1,1,0,0,1,0,0,1,0,0,1,0,1,0,0,0},
		{0,0,1,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0},
		{0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0},
		{0,0,1,1,0,0,0,1,1,1,0,0,1,0,0,0,0,0}
	};
	textcolor(BLACK, WHITE);
	gotoxy(45, 3);
	printf("   ______________________    --\n");
	gotoxy(45, 4);
	printf("  /_________I            ()  --\n");
	gotoxy(45, 5);
	printf("  |    ___-_____________/    --\n");
	gotoxy(45, 6);
	printf("  /  /   //__/\n");
	gotoxy(45, 7);
	printf(" /  /___//\n");
	gotoxy(45, 8);
	printf("/_______/\n");
	for (int i = 0; i < 6; i++) {
		gotoxy(6, i + 3);
		for (int j = 0; j < 18; j++) {
			if (game_name_Gun[i][j] == 1) {
				textcolor(BLACK, RED1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	for (int i = 0; i < 6; i++) {
		gotoxy(6, i + 12);
		for (int j = 0; j < 18; j++) {
			if (game_name_Mas[i][j] == 1) {
				textcolor(BLACK, GRAY1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	for (int i = 0; i < 6; i++) {
		gotoxy(40, i + 12);
		for (int j = 0; j < 18; j++) {
			if (game_name_ter[i][j] == 1) {
				textcolor(BLACK, GRAY1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	textcolor(BLUE1, WHITE);
	gotoxy(32, 20);
	printf("���ӽ��� : s");
	gotoxy(32, 21);
	printf("�������� : q");
	textcolor(BLACK, WHITE);
}
void game_main()
{
	removeCursor();
	game_main_name();
	draw_box2_move(0, 0, 78, 23, "��");
	textcolor(BLACK, WHITE);
}
void game_play_screen() {
	draw_col(60, "��");
	gotoxy(69, 3);
	printf("Gun");
	gotoxy(68, 4);
	printf("Master");
}
//���� ���� ����
void end_name() {
	int end_name_Game[6][24] = {
		{0,1,1,1,1,0,0,0,1,1,0,0,1,0,0,0,0,1,0,1,1,1,1,1},
		{1,0,0,0,0,0,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,0,0},
		{1,0,0,0,0,0,0,1,0,0,1,0,1,0,1,1,0,1,0,1,0,0,0,0},
		{1,0,1,1,1,0,0,1,1,1,1,0,1,0,0,0,0,1,0,1,1,1,1,0},
		{1,0,0,0,1,0,0,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0},
		{0,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0,0,1,0,1,1,1,1,1}
	};
	int end_name_Over[6][24] = {
		{0,1,1,1,0,0,0,1,0,0,1,0,0,1,1,1,1,1,0,1,1,1,1,0},
		{1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,1},
		{1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,1},
		{1,0,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,1,1,1,0},
		{1,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0},
		{0,1,1,1,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,1,0,0,1,1}
	};
	int die_person[6][24] = {
		{0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
		{0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,0,0,1,0,0,0,0,0},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,1,2,2,0,0},
		{2,1,1,1,1,1,2,1,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1},
		{2,2,1,1,1,2,2,2,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,0},
		{0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0}
	};
	for (int i = 0; i < 6; i++) {
		gotoxy(6, i + 2);
		for (int j = 0; j < 24; j++) {
			if (end_name_Game[i][j] == 1) {
				textcolor(BLACK, YELLOW1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	for (int i = 0; i < 6; i++) {
		gotoxy(6, i + 10);
		for (int j = 0; j < 24; j++) {
			if (end_name_Over[i][j] == 1) {
				textcolor(BLACK, YELLOW1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	for (int i = 0; i < 6; i++) {
		gotoxy(6, i + 18);
		for (int j = 0; j < 24; j++) {
			if (die_person[i][j] == 1) {
				textcolor(BLACK, BLACK);
				printf("  ");
			}
			else if (die_person[i][j] == 2) {
				textcolor(BLACK, RED1);
				printf("  ");
			}
			else {
				textcolor(BLACK, WHITE);
				printf("  ");
			}
		}
		printf("\n");
	}
	textcolor(BLACK, WHITE);
}
void end_show() {
	gotoxy(60, 10);
	printf("���� : %d", score);
	gotoxy(60, 14);
	printf("�ٽ��ϱ� : r");
	gotoxy(60, 15);
	printf("������ : q");
}
void end_main() {
	removeCursor();
	end_name();
	end_show();
}
//���� ����ȭ�鿡 ���̴� �Լ���
void show_life() {
	switch (life) {
	case 3:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("��");
		}
		break;
	case 2:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("��");
		}
		for (int i = life; i < 3; i++) {
			printf("��");
		}
		break;
	case 1:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("��");
		}
		for (int i = life; i < 3; i++) {
			printf("��");
		}
		break;
	case 0:
		gotoxy(63, 9);
		printf("Life : ������");
	}
	textcolor(BLACK, WHITE);
}
void show_weapon() {
	gotoxy(63, 13);
	switch (weapon_kind) {
	case 0:
		printf("Weapon : ��");
		break;
	case 1:
		printf("Weapon : ��");
		break;
	case 2:
		printf("Weapon : ��");
		break;
	case 3:
		printf("Weapon : ��");
		break;
	case 4:
		printf("Weapon : ��");
		break;
	}
}
void show_bullet() {
	gotoxy(63, 14);
	switch (weapon_kind) {
	case 0:
		printf("Bullet : ��");
		break;
	case 1:
		printf("Bullet : %02d", bullet[1]);
		break;
	case 2:
		printf("Bullet : %02d", bullet[2]);
		break;
	case 3:
		printf("Bullet : %02d", bullet[3]);
		break;
	case 4:
		printf("Bullet : %02d", bullet[4]);
		break;
	}
}
void show_score() {
	gotoxy(63, 19);
	printf("Score : %d", score);
}
static int oldx = 20, oldy = 10, newx = 20, newy = 10;
//������ ����
//������ ����
void show_item() {
	int x, y;
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (weapon[x][y] != 0) {
				switch (weapon[x][y]) {
				case 1:
					gotoxy(x, y);
					printf("��"); //�ӽŰ� 
					break;
				case 2:
					gotoxy(x, y);
					printf("��"); //����
					//bullet[2]=?
					break;
				case 3:
					gotoxy(x, y);
					printf("��");
					//bullet[3]=?
					break;
				}
			}
		}
	}
}
int what_weapon = 1; // ������ ��� �뵵
void spawn_weapon() {
	int x, y;
	x = (rand() % 58) / 2 * 2;
	y = rand() % (HEIGHT - 1) + 1;  // ���� ����� ���Ѵ�
	//what_weapon = rand() % 3 + 1;// 1, 2, 3 : �� ����
	//what_weapon = 3;
	//�������� 2�������� ����
	if (weapon_num < 2) {
		// ���� ���� ���Ⱑ ���� �÷��̾� ���� ���� ����
		if (zombie[x][y] == 0 && x != oldx && y != oldy && weapon[x][y] == 0) {
			switch (what_weapon) {
			case 1:
				//gotoxy(x, y);
				//printf("��"); //�ӽŰ� 
				weapon[x][y] = 1;
				what_weapon++;
				break;
			case 2:
				//gotoxy(x, y);
				//printf("��"); //����
				weapon[x][y] = 2;
				what_weapon++;
				//bullet[2]=?
				break;
			case 3:
				/*gotoxy(x, y);
				printf("��");*/
				weapon[x][y] = 3;
				what_weapon = 1;
				//bullet[3]=?
				break;
			}
			weapon_num++;
		}
	}
}
//������ ����
void equip_weapon() {
	// ���� ������ ������
	weapon_kind = weapon[oldx][oldy];
	weapon_num--;
	weapon[oldx][oldy] = 0;
	switch (weapon_kind) {
	case 1:
		bullet[1] = 30;
		b = 0;
	case 2:
		bullet[2] = 15;
		c = 0;
	case 3:
		bullet[3] = 5;
	}
	show_weapon();
	show_bullet();
}
//�ѽ��
struct gun {
	int x, y;
	int count;
	int zombie_newx;
	int zombie_newy;
	char direction;
};
struct gun A;// ����
struct gun B[30];// �����
struct gun C[15];// ����
struct gun D[5];// ������
struct gun E[10];// ���� ������ ��
void draw_bullet(struct gun Weapon) {
	textcolor(RED2, WHITE);
	gotoxy(Weapon.x, Weapon.y);
	printf(GUN);
	textcolor(BLACK, WHITE);
}
void erase_bullet(struct gun Weapon) {
	gotoxy(Weapon.x, Weapon.y);
	putchar(BLANK);
}
void draw_bullet_2(struct gun Weapon) {
	textcolor(MAGENTA1, WHITE);
	gotoxy(Weapon.zombie_newx, Weapon.zombie_newy);
	printf(ZGUN);
	textcolor(BLACK, WHITE);
}
void erase_bullet_2(struct gun Weapon) {
	gotoxy(Weapon.zombie_newx, Weapon.zombie_newy);
	putchar(BLANK);
}
void draw_LaserR(struct gun Weapon, int x, int y) {
	textcolor(YELLOW1, WHITE);
	gotoxy(x, y);
	if (x > Weapon.x) {
		for (int i = Weapon.x; i > 0; i -= 2) {
			gotoxy(i, y);
			printf(LaserR);
		}
	}
	else if (x < Weapon.x) {
		for (int i = Weapon.x; i < 58; i += 2) {
			gotoxy(i, y);
			printf(LaserR);
		}
	}
	textcolor(BLACK, WHITE);
}
void erase_LaserR(struct gun Weapon, int x, int y) {
	gotoxy(Weapon.x, Weapon.y);
	if (x > Weapon.x) {
		for (int i = Weapon.x; i > 0; i--) {
			gotoxy(i, y);
			putchar(BLANK);
		}
	}
	else if (x < Weapon.x) {
		for (int i = Weapon.x; i < 58; i++) {
			gotoxy(i, y);
			putchar(BLANK);
		}
	}
}
void draw_LaserC(struct gun Weapon, int x, int y) {
	textcolor(YELLOW1, WHITE);
	gotoxy(Weapon.x, Weapon.y);
	if (y > Weapon.y) {
		for (int i = Weapon.y; i > 0; i--) {
			gotoxy(x, i);
			printf(LaserC);
		}
	}
	else if (y < Weapon.y) {
		for (int i = Weapon.y; i < HEIGHT; i++) {
			gotoxy(x, i);
			printf(LaserC);
		}
	}
	textcolor(BLACK, WHITE);
}
void erase_LaserC(struct gun Weapon, int x, int y) {
	gotoxy(Weapon.x, Weapon.y);
	if (y > Weapon.y) {
		for (int i = Weapon.y; i > 0; i--) {
			gotoxy(x, i);
			putchar(BLANK);
		}
	}
	else if (y < Weapon.y) {
		for (int i = Weapon.y; i < HEIGHT; i++) {
			gotoxy(x, i);
			putchar(BLANK);
		}
	}
}
void move_bullet() {
	erase_bullet(A);
	if (A.x != 0 && A.y != 0) {//A.x, A.y�� 0 0 �϶��� ���� ��������
		switch (A.direction) {
		case UP2:
			if (A.y > 0) {
				if (A.x == oldx && A.y == oldy) {
					A.x = oldx;
					A.y = oldy;
				}
				A.y--;
				draw_bullet(A);
				if (A.y == 0) {
					erase_bullet(A);
					A.x = 0; //
					A.y = 0;
				}
			}
			else {
				A.x = 0;
				A.y = 0;
			}
			break;
		case DOWN2:
			if (A.y < HEIGHT) {
				if (A.x == oldx && A.y == oldy) {
					A.x = oldx;
					A.y = oldy;
				}
				A.y++;
				draw_bullet(A);
				if (A.y == HEIGHT) {
					erase_bullet(A);
					A.x = 0;
					A.y = 0;
				}
			}
			else {
				A.x = 0;
				A.y = 0;
			}
			break;
		case RIGHT2:
			if (A.x < 58) {
				if (A.x == oldx && A.y == oldy) {
					A.x = oldx;
					A.y = oldy;
				}
				A.x += 2;
				draw_bullet(A);
				if (A.x == 60) {
					erase_bullet(A);
					A.x = 0;
					A.y = 0;
				}
			}
			else {
				A.x = 0;
				A.y = 0;
			}
			break;
		case LEFT2:
			if (A.x > 0) {
				if (A.x == oldx && A.y == oldy) {
					A.x = oldx;
					A.y = oldy;
				}
				A.x -= 2;
				draw_bullet(A);
				if (A.x == 0) {
					erase_bullet(A);
					A.x = 0;
					A.y = 0;
				}
			}
			else {
				A.x = 0;
				A.y = 0;
			}
			break;
		}
	}
}
void move_bullet_1() {
	for (int i = 0; i < b; i++) {
		//-1�϶� �Ѿ��� ������
		if (B[i].x != -1 && B[i].x != 0 && B[i].y != 0) {
			switch (B[i].direction) {
			case UP2: //�̰� ���� ���ȴ� �Ұ��� ����
				if (B[i].y > 0) {
					if (B[i].x == oldx && B[i].y == oldy) {
						B[i].x = oldx;
						B[i].y = oldy;
						bullet[1]--;
						show_bullet();
					}
					erase_bullet(B[i]);
					B[i].y--;
					draw_bullet(B[i]);
					if (B[i].y == 0) {
						erase_bullet(B[i]);
						B[i].x = -1; // -1 �̸� �Ѿ��� ������
						show_bullet();
						if (bullet[1] == 0 && B[29].x == -1) {// �Ѿ� �� ���� ��������
							weapon_kind = 0;
							show_weapon();
							show_bullet();
						}
					}
				}
				break;
			case DOWN2:
				if (B[i].y < HEIGHT) {
					if (B[i].x == oldx && B[i].y == oldy) {
						B[i].x = oldx;
						B[i].y = oldy;
						bullet[1]--;
						show_bullet();
					}
					erase_bullet(B[i]);
					B[i].y++;
					draw_bullet(B[i]);
					if (B[i].y == HEIGHT) {
						erase_bullet(B[i]);
						B[i].x = -1; // -1 �̸� �Ѿ��� ������
						if (bullet[1] == 0 && B[29].x == -1) {// �Ѿ� �� ���� ��������
							weapon_kind = 0;
							show_weapon();
							show_bullet();
						}
					}
				}
				break;
			case RIGHT2:
				if (B[i].x < 58) {
					if (B[i].x == oldx && B[i].y == oldy) {
						B[i].x = oldx;
						B[i].y = oldy;
						bullet[1]--;
						show_bullet();
					}
					erase_bullet(B[i]);
					B[i].x += 2;
					draw_bullet(B[i]);
					if (B[i].x == 58) {
						erase_bullet(B[i]);
						B[i].x = -1; // -1 �̸� �Ѿ��� ������
						if (bullet[1] == 0 && B[29].x == -1) {// �Ѿ� �� ���� ��������
							weapon_kind = 0;
							show_weapon();
							show_bullet();
						}
					}
				}
				break;
			case LEFT2:
				if (B[i].x > 0) {
					if (B[i].x == oldx && B[i].y == oldy) {
						B[i].x = oldx;
						B[i].y = oldy;
						bullet[1]--;
						show_bullet();
					}
					erase_bullet(B[i]);
					B[i].x -= 2;
					draw_bullet(B[i]);
					if (B[i].x == 0) {
						erase_bullet(B[i]);
						B[i].x = -1; // -1 �̸� �Ѿ��� ������
						if (bullet[1] == 0 && B[29].x == -1) {// �Ѿ� �� ���� ��������
							weapon_kind = 0;
							show_weapon();
							show_bullet();
						}
					}
				}
				break;
			}
		}
	}
}
void move_bullet_2() {
	for (int i = 0; i < c; i = i + 3) {
		if ((C[i].x != -1 || C[i + 1].x != -1 || C[i + 2].x != -1) &&
			C[i].x != 0 && C[i].y != 0) {
			switch (C[i].direction) {
			case UP2: //�̰� ���� ���ȴ� �Ұ��� ����
				if (C[i].y > 0) {
					if (C[i].x == oldx && C[i].y == oldy) {
						C[i].x = oldx;
						C[i].y = oldy;
						bullet[2] = bullet[2] - 3;
						show_bullet();
					}
					if (C[i].x != -1) {
						erase_bullet(C[i]);
						C[i].y--;
						draw_bullet(C[i]);
					}
					if (C[i + 1].x != -1) {
						erase_bullet(C[i + 1]);
						C[i + 1].y--;
						draw_bullet(C[i + 1]);
					}
					if (C[i + 2].x != -1) {
						erase_bullet(C[i + 2]);
						C[i + 2].y--;
						draw_bullet(C[i + 2]);
					}
					if (C[i].y == 0) {
						erase_bullet(C[i]);
						C[i].x = -1; // -1 �̸� �Ѿ��� ������
					}
					if (C[i + 1].y == 0) {
						erase_bullet(C[i + 1]);
						C[i + 1].x = -1;
					}
					if (C[i + 2].y == 0) {
						erase_bullet(C[i + 2]);
						C[i + 2].x = -1;
					}
					if (bullet[2] == 0 && C[12].x == -1 &&
						C[13].x == -1 && C[14].x == -1) {
						weapon_kind = 0;
						show_weapon();
						show_bullet();
					}
				}
				break;
			case DOWN2:
				if (C[i].y < HEIGHT) {
					if (C[i].x == oldx && C[i].y == oldy) {
						C[i].x = oldx;
						C[i].y = oldy;
						bullet[2] = bullet[2] - 3;
						show_bullet();
					}
					if (C[i].x != -1) {
						erase_bullet(C[i]);
						C[i].y++;
						draw_bullet(C[i]);
					}
					if (C[i + 1].x != -1) {
						erase_bullet(C[i + 1]);
						C[i + 1].y++;
						draw_bullet(C[i + 1]);
					}
					if (C[i + 2].x != -1) {
						erase_bullet(C[i + 2]);
						C[i + 2].y++;
						draw_bullet(C[i + 2]);
					}
					if (C[i].y == HEIGHT) {
						erase_bullet(C[i]);
						C[i].x = -1; // -1 �̸� �Ѿ��� ������
					}
					if (C[i + 1].y == HEIGHT) {
						erase_bullet(C[i + 1]);
						C[i + 1].x = -1;
					}
					if (C[i + 2].y == HEIGHT) {
						erase_bullet(C[i + 2]);
						C[i + 2].x = -1;
					}
					if (bullet[2] == 0 && C[12].x == -1 &&
						C[13].x == -1 && C[14].x == -1) {
						weapon_kind = 0;
						show_weapon();
						show_bullet();
					}
				}
				break;
			case RIGHT2:
				if (C[i].x < 58) {
					if (C[i].x == oldx && C[i].y == oldy) {
						C[i].x = oldx;
						C[i].y = oldy;
						bullet[2] = bullet[2] - 3;
						show_bullet();
					}
					if (C[i].x != -1) {
						erase_bullet(C[i]);
						C[i].x += 2;
						draw_bullet(C[i]);
					}
					if (C[i + 1].x != -1) {
						erase_bullet(C[i + 1]);
						C[i + 1].x += 2;
						draw_bullet(C[i + 1]);
					}
					if (C[i + 2].x != -1) {
						erase_bullet(C[i + 2]);
						C[i + 2].x += 2;
						draw_bullet(C[i + 2]);
					}
					if (C[i].x == 58) {
						erase_bullet(C[i]);
						C[i].x = -1; // -1 �̸� �Ѿ��� ������
					}
					if (C[i + 1].x == 58) {
						erase_bullet(C[i + 1]);
						C[i + 1].x = -1;
					}
					if (C[i + 2].x == 58) {
						erase_bullet(C[i + 2]);
						C[i + 2].x = -1;
					}
					if (bullet[2] == 0 && C[12].x == -1 &&
						C[13].x == -1 && C[14].x == -1) {
						weapon_kind = 0;
						show_weapon();
						show_bullet();
					}
				}
				break;
			case LEFT2:
				if (C[i].x >= -1) {
					if (C[i].x == oldx && C[i].y == oldy) {
						C[i].x = oldx;
						C[i].y = oldy;
						bullet[2] = bullet[2] - 3;
						show_bullet();
					}
					if (C[i].x != -1) {
						erase_bullet(C[i]);
						C[i].x -= 2;
						draw_bullet(C[i]);
					}
					if (C[i + 1].x != -1) {
						erase_bullet(C[i + 1]);
						C[i + 1].x -= 2;
						draw_bullet(C[i + 1]);
					}
					if (C[i + 2].x != -1) {
						erase_bullet(C[i + 2]);
						C[i + 2].x -= 2;
						draw_bullet(C[i + 2]);
					}
					if (C[i].x == 0) {
						erase_bullet(C[i]);
						C[i].x = -1; // -1 �̸� �Ѿ��� ������
					}
					if (C[i + 1].x == 0) {
						erase_bullet(C[i + 1]);
						C[i + 1].x = -1;
					}
					if (C[i + 2].x == 0) {
						erase_bullet(C[i + 2]);
						C[i + 2].x = -1;
					}
					if (bullet[2] == 0 && C[12].x == -1 &&
						C[13].x == -1 && C[14].x == -1) {
						weapon_kind = 0;
						show_weapon();
						show_bullet();
					}
				}
				break;
			}
		}
	}
}
void move_bullet_3() {
	int p = 0, q = 0;
	if (D[d].direction == UP2) {
		D[d].y--;
		p = oldx;
		q = oldy;
		draw_LaserC(D[d], p, q);
		Sleep(200);
		erase_LaserC(D[d], p, q);
		for (int i = D[d].y; i >= 0; i--) {
			if (zombie[D[d].x][i] != 0) {
				switch (zombie[D[d].x][i]) {
				case 1:
					score = score + 10;
					show_score();
					break;
				case 2:
					score = score + 30;
					show_score();
					break;
				case 3:
					score = score + 50;
					show_score();
					break;
				}
				zombie[D[d].x][i] = 0;
				zombie_count--;
			}
		}
		d++;
		bullet[3]--;
		show_bullet();
		if (bullet[3] == 0) {
			weapon_kind = 0;
			show_weapon();
			show_bullet();
		}
	}
	else if (D[d].direction == DOWN2) {
		D[d].y++;
		p = oldx;
		q = oldy;
		draw_LaserC(D[d], p, q);
		Sleep(200);
		erase_LaserC(D[d], p, q);
		for (int i = D[d].y; i <= HEIGHT; i++) {
			if (zombie[D[d].x][i] != 0) {
				switch (zombie[D[d].x][i]) {
				case 1:
					score = score + 10;
					show_score();
					break;
				case 2:
					score = score + 30;
					show_score();
					break;
				case 3:
					score = score + 50;
					show_score();
					break;
				}
				zombie[D[d].x][i] = 0;
				zombie_count--;
			}
		}
		d++;
		bullet[3]--;
		show_bullet();
		if (bullet[3] == 0) {
			weapon_kind = 0;
			show_weapon();
			show_bullet();
		}
	}
	else if (D[d].direction == RIGHT2) {
		D[d].x += 2;
		p = oldx;
		q = oldy;
		draw_LaserR(D[d], p, q);
		Sleep(200);
		erase_LaserR(D[d], p, q);
		for (int i = D[d].x; i <= 58; i++) {
			if (zombie[i][D[d].y] != 0) {
				switch (zombie[i][D[d].y]) {
				case 1:
					score = score + 10;
					show_score();
					break;
				case 2:
					score = score + 30;
					show_score();
					break;
				case 3:
					score = score + 50;
					show_score();
					break;
				}
				zombie[i][D[d].y] = 0;
				zombie_count--;
			}
		}
		d++;
		bullet[3] = bullet[3] - 1;
		show_bullet();
		if (bullet[3] == 0) {
			weapon_kind = 0;
			show_weapon();
			show_bullet();
		}
	}
	else if (D[d].direction == LEFT2) {
		D[d].x -= 2;
		p = oldx;
		q = oldy;
		draw_LaserR(D[d], p, q);
		Sleep(200);
		erase_LaserR(D[d], p, q);
		for (int i = D[d].x; i >= 0; i--) {
			if (zombie[i][D[d].y] != 0) {
				switch (zombie[i][D[d].y]) {
				case 1:
					score = score + 10;
					show_score();
					break;
				case 2:
					score = score + 30;
					show_score();
					break;
				case 3:
					score = score + 50;
					show_score();
					break;
				}
				zombie[i][D[d].y] = 0;
				zombie_count--;
			}
		}
		d++;
		bullet[3] = bullet[3] - 1;
		show_bullet();
		if (bullet[3] == 0) {
			weapon_kind = 0;
			show_weapon();
			show_bullet();
		}
	}
	if (d == 5) {
		for (int i = 0; i < 5; i++) {
			D[i].direction = 0;
		}
		d = 0;
	}
}
//���� ���� (���Ḯ��Ʈ�� ������ �� �������� :x,y life ���� �̷��� element
//����. ����鳢�� �Ȱ�ġ�� �Ϸ��� �ߴµ� ���� �̻��մϴ�
void init_gun_zombie(int x, int y) { //����3�Ѿ� �ʱ�ȭ
	E[e].x = x;
	E[e].y = y;
	E[e].zombie_newx = x;
	E[e].zombie_newy = y;
	E[e].count = 0;
	e++;
	if (e == 10) {
		e = 0;
	}
}
void produce_zombie() //���� ����
{
	int x, y, r;
	x = (rand() % 56) / 2 * 2 + 2;//������ 56����
	y = rand() % (HEIGHT - 1) + 1;  // ���� ����� ���Ѵ�
	r = rand() % 100; //���� ���� Ȯ��
	//�ѽ�� ����� 10���� ����
	check_e = 0;
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (zombie[i][j] == 3)
				check_e++;
		}
	}
	//���� ���� zombie_count ����
	if (zombie_count <= restrict_zombie) {//restrict_zombie = ���� ���� �� �ִ� ��
		if (r > zombie_1_P) { //�Ϲ� ���� 50�ۼ�Ʈ Ȯ���� ����
			if (zombie[x][y] == 0) {
				zombie[x][y] = 1;
				zombie_count++;
			}
		}
		else if (r > zombie_2_P) { // ��� ���� 25�ۼ�Ʈ Ȯ���� ����
			if (zombie[x][y] == 0) {
				zombie[x][y] = 2;
				zombie_count++;
			}
		}
		else if (check_e < 10) {  //���� ���� 25�ۼ�Ʈ Ȯ���� ����, ��� 10���� ������ ���´�
			if (zombie[x][y] == 0) {
				zombie[x][y] = 3;
				zombie_count++;
				init_gun_zombie(x, y);
			}
		}
		textcolor(BLACK, WHITE);
	}
}
// ���� 1�� �Լ���
void show_zombie_1() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 1) {
				gotoxy(x, y);
				textcolor(BLACK, WHITE);
				printf(Zombie);
				textcolor(BLACK, WHITE);
			}
		}
	}
}
void erase_zombie_1() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 1) {
				gotoxy(x, y);
				printf("  ");
			}
		}
	}
}
void move_zombie_1() {// 1�� ���� ������ �Լ�// ������ move_zombie_1�� ����~
	int x, y;
	int zombie_newx = 0;
	int zombie_newy = 0;
	int newzombie[60][HEIGHT] = { 0 };
	if (zombie_count == 0)
		return;
	erase_zombie_1(); // �ϴ� ���� 1 �� ����
	for (x = 0; x < 60; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 1) {//1�� �����϶�
				if ((oldx - x) > 0) { //������� ���ο� ��ǥ �����
					zombie_newx = x + 2;
					if (zombie_newx > 60)
						zombie_newx = 58;
				}
				else if ((oldx - x) < 0) {
					zombie_newx = x - 2;
					if (zombie_newx <= 0)
						zombie_newx = 0;
				}
				else {
					zombie_newx = x;
				}
				if ((oldy - y) > 0) {
					zombie_newy = y + 1;
					if (zombie_newy >= HEIGHT)
						zombie_newy = HEIGHT;
				}
				else if ((oldy - y) < 0) {
					zombie_newy = y - 1;
					if (zombie_newy <= 0)
						zombie_newy = 0;
				}
				else {
					zombie_newy = y;
				}
				zombie[x][y] = 0;//������ �ִ� ���� ���ֱ�
				if (zombie_newx >= 0 && zombie_newx < 60 && zombie_newy >= 0 && zombie_newy < HEIGHT) {
					if (zombie[zombie_newx][zombie_newy] == 0 && newzombie[zombie_newx][zombie_newy] == 0) {
						// ���ο� ��ġ�� ���� ������ ���� �̵�
						newzombie[zombie_newx][zombie_newy] = 1;
					}
					else {
						// ���ο� ��ġ�� �̹� ���� ������ ���� ��ġ�� ����
						newzombie[x][y] = 1;
					}
				}
				else {
					// ���� ������ ����� ���� ��ġ�� ����
					newzombie[x][y] = 1;
				}
			}
		}
	}
	for (x = 0; x < 60; x++) {//newzombie�� ���� �Ѱ��� ������ �ٷ� zombie���� �ָ�
		//�ߺ��Ǳ� ������ ���� �ְ� �־������
		for (y = 0; y < HEIGHT; y++) {
			if (newzombie[x][y] == 1) {
				zombie[x][y] = newzombie[x][y];
				newzombie[x][y] = 0;
			}
		}
	}
	show_zombie_1();//�Ű��� ��ǥ���� ���� 1 ���
}
// ���� 2�� �Լ���
void show_zombie_2() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 2) {
				gotoxy(x, y);
				textcolor(BLUE1, WHITE);
				printf(Zombie);
				textcolor(BLACK, WHITE);
			}
		}
	}
}
void erase_zombie_2() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 2) {
				gotoxy(x, y);
				printf("  ");
			}
		}
	}
}
void move_zombie_2() {// 2�� ���� ������ �Լ�
	int x, y;
	int zombie_newx = 0;
	int zombie_newy = 0;
	int newzombie[60][HEIGHT] = { 0 };
	if (zombie_count == 0)
		return;
	erase_zombie_2(); // �ϴ� ���� 2 �� ����
	for (x = 0; x < 60; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 2) {//2�� �����϶�
				if ((oldx - x) > 0) { //������� ���ο� ��ǥ �����
					zombie_newx = x + 2;
					if (zombie_newx > 60)
						zombie_newx = 58;
				}
				else if ((oldx - x) < 0) {
					zombie_newx = x - 2;
					if (zombie_newx <= 0)
						zombie_newx = 0;
				}
				else {
					zombie_newx = x;
				}
				if ((oldy - y) > 0) {
					zombie_newy = y + 1;
					if (zombie_newy >= HEIGHT)
						zombie_newy = HEIGHT;
				}
				else if ((oldy - y) < 0) {
					zombie_newy = y - 1;
					if (zombie_newy <= 0)
						zombie_newy = 0;
				}
				else {
					zombie_newy = y;
				}
				zombie[x][y] = 0;//������ �ִ� ���� ���ֱ�
				if (zombie_newx >= 0 && zombie_newx < 60 && zombie_newy >= 0 && zombie_newy < HEIGHT) {
					if (zombie[zombie_newx][zombie_newy] == 0 && newzombie[zombie_newx][zombie_newy] == 0) {
						// ���ο� ��ġ�� ���� ������ ���� �̵�
						newzombie[zombie_newx][zombie_newy] = 2;
					}
					else {
						// ���ο� ��ġ�� �̹� ���� ������ ���� ��ġ�� ����
						newzombie[x][y] = 2;
					}
				}
				else {
					// ���� ������ ����� ���� ��ġ�� ����
					newzombie[x][y] = 2;
				}
			}
		}
	}
	for (x = 0; x < 60; x++) {//newzombie�� ���� �Ѱ��� ������ �ٷ� zombie���� �ָ�
		//�ߺ��Ǳ� ������ ���� �ְ� �־������
		for (y = 0; y < HEIGHT; y++) {
			if (newzombie[x][y] == 2) {
				zombie[x][y] = newzombie[x][y];
				newzombie[x][y] = 0;
			}
		}
	}
	show_zombie_2();//�Ű��� ��ǥ���� ���� 2 ���
}
// ���� 3�� �Լ���
void show_zombie_3() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 3) {
				gotoxy(x, y);
				textcolor(RED1, WHITE);
				printf(Zombie);
				textcolor(BLACK, WHITE);
			}
		}
	}
}
void erase_zombie_3() {
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 3) {
				gotoxy(x, y);
				printf("  ");
			}
		}
	}
}
void move_zombie_3() {// 3�� ���� �Ѿ� ������ �Լ�
	for (int i = 0; i < 10; i++) {
		if (E[i].zombie_newx != 0 && E[i].zombie_newy != 0) {//������� ����
			erase_bullet_2(E[i]);
			show_zombie_3(); //���� 3�� ������ �ִ´�
			if (oldx > E[i].zombie_newx) {
				E[i].zombie_newx += 2;
			}
			else if (oldx < E[i].zombie_newx) {
				E[i].zombie_newx -= 2;
			}
			if (oldy > E[i].zombie_newy) {
				E[i].zombie_newy += 1;
			}
			else if (oldy < E[i].zombie_newy) {
				E[i].zombie_newy -= 1;
			}
			draw_bullet_2(E[i]);
			if (zombie[E[i].x][E[i].y] == 0)
				E[i].count++;
			if (E[i].count == 7 && zombie[E[i].x][E[i].y] != 3) { //7�� �����̸� �Ѿ� �����
				erase_bullet_2(E[i]);
				E[i].zombie_newx = 0;
				E[i].zombie_newy = 0;
			}
		}
	}
}
//�Ѿ˿� ������ ���� ���� ����
void die_zombie() {
	// �Ѿ��� �ִ� ���� ���� ����
	   //���⼭�� switch�ؼ� erase_bullet_1 �̷������� �ؾ��ҵ�
	erase_bullet(A);
	zombie_count--;
	switch (zombie[A.x][A.y]) { // ���� ���� ���� ���� ����
	case 1:
		score = score + 10;
		show_score();
		break;
	case 2:
		score = score + 30;
		show_score();
		break;
	case 3:
		score = score + 50;
		show_score();
		break;
	}
	zombie[A.x][A.y] = 0; //���� ����
	A.x = 0; //�ٽ� �Ѿ��� ���� �� �ֵ��� 0,0 ���� �ʱ�ȭ
	A.y = 0;
}
void die_zombie_1(int n) {
	erase_bullet(B[n]);
	zombie_count--;
	switch (zombie[B[n].x][B[n].y]) { // ���� ���� ���� ���� ����
	case 1:
		score = score + 10;
		show_score();
		break;
	case 2:
		score = score + 30;
		show_score();
		break;
	case 3:
		score = score + 50;
		show_score();
		break;
	}
	zombie[B[n].x][B[n].y] = 0;
	B[n].x = -1; // ���� ������ �Ѿ��� ������
}
void die_zombie_2(int n) {
	erase_bullet(C[n]);
	zombie_count--;
	switch (zombie[C[n].x][C[n].y]) { // ���� ���� ���� ���� ����
	case 1:
		score = score + 10;
		show_score();
		break;
	case 2:
		score = score + 30;
		show_score();
		break;
	case 3:
		score = score + 50;
		show_score();
		break;
	}
	zombie[C[n].x][C[n].y] = 0;
	C[n].x = -1; // ���� ������ �Ѿ��� ������
}
// ����� �ε����� ������ ���� �Լ�
void die_player() {
	//�ε����� ���� ����
	zombie[oldx][oldy] = 0;
	zombie_count--;
	life--; //������ ����
	show_life();
}
//�÷��̾� ����
void die_player_2(int n) {
	erase_bullet_2(E[n]);
	//���� 3�� �ٽ� �� �� �ְ� �ʱ�ȭ
	E[n].zombie_newx = E[n].x;
	E[n].zombie_newy = E[n].y;
	life--;
	show_life();
}
void control_game() {//���̵� ���� : ���� �ӵ� ����
	restrict_zombie += 2;
	zombie1_frame_sync -= 1;
	zombie2_frame_sync -= 1;
	zombie3_frame_sync -= 1;
	stage++;
}
//void control_game2() {//���̵� ���� : ���� �� ����, ������ Ȯ�� ����
//	restrict_zombie += 3;
//	zombie_1_P += 5;
//	zombie_2_P += 5;
//}
void player(unsigned char ch)
{
	int move_flag = 0;
	static unsigned char last_ch = 0;

	if (called[0] == 0) { // ó�� �Ǵ� Restart
		oldx = 20, oldy = 10, newx = 20, newy = 10;
		putstar(oldx, oldy, PLAYER);
		called[0] = 1;
		last_ch = 0;
		ch = 0;
	}

	switch (ch) {
	case UP:
		if (oldy >= 0) {
			newy = oldy - 1;
			if (oldy == 0)
				newy = 0;
		}
		move_flag = 1;
		break;
	case DOWN:
		if (oldy <= HEIGHT) {
			newy = oldy + 1;
			if (oldy == HEIGHT)
				newy = HEIGHT;
		}
		move_flag = 1;
		break;
	case LEFT:
		if (oldx >= 0) {
			newx = oldx - 2;
			if (oldx == 0)
				newx = 0;
		}
		move_flag = 1;
		break;
	case RIGHT:
		if (oldx <= 58) {
			newx = oldx + 2;
			if (oldx == 58)
				newx = 58;
		}
		move_flag = 1;
		break;
	default:
		newx = oldx;
		newy = oldy;
	}
	if (move_flag) {
		erasestar(oldx, oldy); // ������ ��ġ�� * �� �����
		putstar(newx, newy, PLAYER); // ���ο� ��ġ���� * �� ǥ���Ѵ�.
		oldx = newx; // ������ ��ġ�� ����Ѵ�.
		oldy = newy;
	}
	//Sleep(Delay);
}
//���� �ʱ�ȭ
void init_game() {
	life = 3;
	score = 0;
	weapon_kind = 0;
	weapon_num = 0;
	restrict_zombie = 4;
	zombie_1_P = 50;
	zombie_2_P = 25;
	zombie1_frame_sync = 50;
	zombie2_frame_sync = 20;
	zombie3_frame_sync = 35;
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			zombie[x][y] = 0;
			weapon[x][y] = 0;
		}
	}
	for (int k = 0; k < 10; k++) {
		E[k].zombie_newx = 0;
		E[k].zombie_newy = 0;
	}
	game_play_screen();
	show_life();
	show_weapon();
	show_bullet();
	show_score();
}
//����ã��
// �ؾ��Ұ� ���� �Ѿ� �� ���� �ȹٲ�µ�
// �Ѿ� �� ���� �߰��� ���ߴ°�
// ���̵� �ø���
void main(int argc, char* argv[])
{
	unsigned char ch;
	static char last_ch;
	int keep_moving;
	int frame_count = 0;
	srand(time(NULL));
	draw_box2(0, 0, 78, 23, "��");
	PlaySound(TEXT("wave_file.wav"), 0, SND_FILENAME | SND_ASYNC);
	//if (argc == 1) {
	//	// bgm ���
	//	PlaySound(TEXT("bgm.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//	// ���� �����ϰ� ������ ����ɶ����� ��ٸ���.
	//	spawnl(P_WAIT, argv[0], argv[0], "gamerun", NULL);
	//	// ������ ����Ǹ� bgm process�� ����
	//	exit(0);
	//}
	while (1) {
		game_main();
		if (kbhit()) {
			ch = getch();
			if (ch == 's' || ch == 'q')
				break;
		}
	}
	if (ch == 's')
		goto START;
	else if (ch == 'q')
		return 0;
START:
	cls(WHITE, BLACK);
	init_game();
	PlaySound(TEXT("empty.wav"), 0, SND_FILENAME | SND_ASYNC);
	while (1) {
		show_item();
		//�⺻������ ���� ���̱�
		if (zombie[A.x][A.y] != 0) { //�Ѿ��̶� ���� ��ǥ�� ������ ���� ����)
			die_zombie();
		}
		//��������� ���� ���̱�
		for (int i = 0; i < 30; i++) {
			if (zombie[B[i].x][B[i].y] != 0) { //�Ѿ��̶� ���� ��ǥ�� ������ ���� ����)
				die_zombie_1(i);
			}
		}
		//�������� ���� ���̱�
		for (int i = 0; i < 15; i = i + 3) {
			if (zombie[C[i].x][C[i].y] != 0) { //�Ѿ��̶� ���� ��ǥ�� ������ ���� ����
				die_zombie_2(i);
			}
			else if (zombie[C[i + 1].x][C[i + 1].y] != 0) {
				die_zombie_2(i + 1);
			}
			else if (zombie[C[i + 2].x][C[i + 2].y] != 0) {
				die_zombie_2(i + 2);
			}
		}
		//�������� ���� ���̱�� move_bullet_3�� ���Ե�

		if (zombie[oldx][oldy] != 0) {//��ǥ�� ����� ������ ������ ����
			die_player();
			PlaySound(TEXT("damage.wav"), 0, SND_FILENAME | SND_ASYNC);
			if (life == 0)
				goto END;
		}
		//���� 3�� ����ü�� ������ ������ ����
		for (int i = 0; i < 10; i++) {
			if (E[i].zombie_newx == oldx && E[i].zombie_newy == oldy) {
				die_player_2(i);
				PlaySound(TEXT("damage.wav"), 0, SND_FILENAME | SND_ASYNC);
				if (life == 0)
					goto END;
			}
		}
		if (weapon[oldx][oldy] != 0) {//��ǥ�� ������ ������ ���� ����
			equip_weapon();
		}
		if (frame_count % zombie_appear == 0) { //zombie_appear�� ���� ���� ����?
			produce_zombie();
		}
		if (kbhit() == 1) {  // Ű���尡 ������ ������
			ch = getch(); // key ���� �д´�
			// ESC ������ ���α׷� ����
			if (ch == ESC) {
				gotoxy(25, 11);
				exit(0);
				break;
			}
			if (ch == SPECIAL1) { // ���� Ư��Ű
				// ���� ��� UP key�� ��� 0xe0 0x48 �ΰ��� ���ڰ� ���´�.
				ch = getch();
				// Player�� ����Ű�� �����δ�.
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player(ch);
					//player(last_ch);
					break;
				default: // ���� ��ȯ�� �ƴϸ�
					player(0);
				}
			}
			else {
				switch (ch) {
				case UP2:
				case DOWN2:
				case RIGHT2:
				case LEFT2:
					switch (weapon_kind) {
					case 0:
						if (A.x == 0 && A.y == 0) {
							A.direction = ch;
							A.x = oldx;
							A.y = oldy;
							player(0);
							PlaySound(TEXT("pistol.wav"), 0, SND_FILENAME | SND_ASYNC);
						}
						break;
					case 1:
						B[b].direction = ch;
						B[b].x = oldx;
						B[b].y = oldy;
						b++;
						player(0);
						PlaySound(TEXT("pistol2.wav"), 0, SND_FILENAME | SND_ASYNC);
						break;
					case 2:
						C[c].direction = ch;
						C[c].x = oldx;
						C[c].y = oldy;
						PlaySound(TEXT("shotgun1.wav"), 0, SND_FILENAME | SND_ASYNC);
						if (ch == UP2 || ch == DOWN2) {
							C[c + 1].direction = ch;
							C[c + 1].x = oldx + 2;
							C[c + 1].y = oldy;
							C[c + 2].direction = ch;
							C[c + 2].x = oldx - 2;
							C[c + 2].y = oldy;
						}
						else if (ch == RIGHT2 || ch == LEFT2) {
							C[c + 1].direction = ch;
							C[c + 1].x = oldx;
							C[c + 1].y = oldy + 1;
							C[c + 2].direction = ch;
							C[c + 2].x = oldx;
							C[c + 2].y = oldy - 1;
						}
						c = c + 3;
						player(0);
						break;
					case 3:
						D[d].direction = ch;
						D[d].x = oldx;
						D[d].y = oldy;
						PlaySound(TEXT("laser.wav"), 0, SND_FILENAME | SND_ASYNC);
						player(0);
						break;
					}
					break;
				default:
					player(0);
				}
			}
		}
		else {
			player(0);
		}

		if (frame_count % bullet_frame_sync == 0) {
			//switch�� �� ������ ������.
			switch (weapon_kind) {
			case 0:
				move_bullet(); // �Ѿ��� player()�ʹ� ������ �̵���Ų��.
				putstar(oldx, oldy, PLAYER);
				break;
			case 1:
				move_bullet_1();
				putstar(oldx, oldy, PLAYER);
				break;
			case 2:
				move_bullet_2();
				putstar(oldx, oldy, PLAYER);
				break;
			case 3:
				move_bullet_3();
				putstar(oldx, oldy, PLAYER);
				break;
			}
		}
		if (frame_count % zombie1_frame_sync == 0)
			move_zombie_1(); //zombie1�� ��ġ�� �����Ѵ�.
		if (frame_count % zombie2_frame_sync == 0)
			move_zombie_2(); //zombie2�� ��ġ�� �����Ѵ�.
		if (frame_count % zombie3_frame_sync == 0)
			move_zombie_3(); //zombie3�� �ð� ���� ����ü�� ������.
		if (frame_count % weapon_frame_sync == 0)
			spawn_weapon();

		Sleep(Delay); // Delay ���� ���̰�
		frame_count++; // frame_count ������ �ӵ� ������ �Ѵ�.
		//���� ������ �Ѿ�� ������ �� ����������� ����****************)
		if (score / 700 > stage)// ������ 500�� ���� ���� ���ǵ带 �ø���
			control_game();
		//if (score / 500 > stage2)// ������ 500�� ���� ���� ���� �ø��� Ȯ���� �����Ѵ�
		//	control_game2(); 
	}
END:
	Sleep(300);
	cls(WHITE, BLACK);
	PlaySound(TEXT("ending.wav"), 0, SND_FILENAME | SND_ASYNC);
	while (1) {
		removeCursor();
		end_main();
		if (kbhit()) {
			ch = getch();
			if (ch == 'r' || ch == 'q')
				break;
		}
	}
	if (ch == 'r')
		goto START;
	if (ch == 'q')
		return 0;
}
