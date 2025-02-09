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

// 색상 정의
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

#define PLAYER "♀"
#define GUN "ㆍ"
#define ZGUN "ο"
#define LaserR "━"
#define LaserC "┃"
#define Zombie "¶"
#define BLANK '  ' // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define UP2		'w'//총쏘기는 AWSD 로 방향키 대신
#define DOWN2	's'
#define LEFT2	'a'
#define RIGHT2	'd'

#define WIDTH 80
#define HEIGHT 24
#define MAX_B = 100;


int Delay = 10; // 100 msec delay, 이 값을 줄이면 속도가 빨라진다.
int keep_moving = 1; // 1:계속이동, 0:한칸씩이동.
int called[2];
int life = 3;
int score = 0;
int restrict_zombie = 1;// 좀비 제한 수 시간 지나면 늘어남
int weapon[60][HEIGHT] = { 0 }; // 맵에 무기 아이템이 무작위로 생성
int zombie[60][HEIGHT] = { 0 }; // 맵에 좀비가 무작위로 생성
int weapon_kind;
int weapon_num; // 맵에 나오는 무기의 수(2개까지만 생성)
int b = 0;
int c = 0;
int d = 0;
int e = 0;
int check_e;

int stage = 0; // 난이도 변수
int stage2 = 0;

int zombie_1_P;
int zombie_2_P;
int bullet[5] = { 0 }; // 총 종류에 따라(0~4) 총알 수가 다르다

int zombie_count = 0; // 좀비 수
int zombie_appear = 50;

int frame_count = 0;
int bullet_frame_sync = 2; //총알 나가는 속도
int zombie1_frame_sync = 50; // 좀비 1
int zombie2_frame_sync = 10; // 좀비 2
int zombie3_frame_sync = 40; // 좀비 3
int weapon_frame_sync = 100; // 무기 아이템 생성
//무기정보
/*weapon
0 : 권총(P) : 기본
1 : 기관총(M) : 총알 속도 빠르다, 벽에 안가도 쏠 수 있다?
2 : 샷건(S) : 위 아래 3발씩 나감
3 : 레이저(L) : 속도 겁나 빠르게 하고 총알을 일직선으로 쏘게함
4 : 폭탄(B) : 못할듯
*/

//좀비정보
/* zombie
1 : 일반 좀비
2 : 무언가 쏘는 좀비
3 : 속도가 빠른 좀비
*/
//교수님 코드
void removeCursor(void) { // 커서를 안보이게 한다

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
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
// 화면 지우기고 원하는 배경색으로 설정한다.
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
	// 완성할 수 있으면 할 것
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
//세로 그리기
void draw_col(int x, char* ch) {
	int i;
	for (i = 0; i <= HEIGHT; i++) {
		gotoxy(x, i);
		printf("%s", ch);
	}
}
//문자로 박스 그리기
void draw_box2_move(int x1, int y1, int x2, int y2, char* ch) {
	int i = rand() % 6 + 1;
	textcolor(i, WHITE);
	draw_box2(x1, y1, x2, y2, ch);
	Sleep(300);
}
//선으로 박스 그리기
void draw_box_line(int x1, int y1, int x2, int y2)
{
	int x, y;
	//int len = strlen(ch);
	for (x = x1; x <= x2; x += 2) { // 한글은 2칸씩 차지한다.
		gotoxy(x, y1);
		printf("%s", "─"); // ㅂ 누르고 한자키 누르고 선택
		gotoxy(x, y2);
		printf("%s", "─");
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%s", "│");
		gotoxy(x2, y);
		printf("%s", "│");
	}
	gotoxy(x1, y1); printf("┌");
	gotoxy(x1, y2); printf("└");
	gotoxy(x2, y1); printf("┐");
	gotoxy(x2, y2); printf("┘");
}
//게임 메인에 쓰인 함수들
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
	printf("게임시작 : s");
	gotoxy(32, 21);
	printf("게임종료 : q");
	textcolor(BLACK, WHITE);
}
void game_main()
{
	removeCursor();
	game_main_name();
	draw_box2_move(0, 0, 78, 23, "★");
	textcolor(BLACK, WHITE);
}
void game_play_screen() {
	draw_col(60, "│");
	gotoxy(69, 3);
	printf("Gun");
	gotoxy(68, 4);
	printf("Master");
}
//게임 엔딩 메인
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
	printf("점수 : %d", score);
	gotoxy(60, 14);
	printf("다시하기 : r");
	gotoxy(60, 15);
	printf("나가기 : q");
}
void end_main() {
	removeCursor();
	end_name();
	end_show();
}
//게임 실행화면에 쓰이는 함수들
void show_life() {
	switch (life) {
	case 3:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("♥");
		}
		break;
	case 2:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("♥");
		}
		for (int i = life; i < 3; i++) {
			printf("♡");
		}
		break;
	case 1:
		gotoxy(63, 9);
		printf("Life : ");
		draw_box_line(65, 2, 75, 5);
		gotoxy(70, 9);
		textcolor(RED1, WHITE);
		for (int i = 0; i < life; i++) {
			printf("♥");
		}
		for (int i = life; i < 3; i++) {
			printf("♡");
		}
		break;
	case 0:
		gotoxy(63, 9);
		printf("Life : ♡♡♡");
	}
	textcolor(BLACK, WHITE);
}
void show_weapon() {
	gotoxy(63, 13);
	switch (weapon_kind) {
	case 0:
		printf("Weapon : ⓟ");
		break;
	case 1:
		printf("Weapon : ⓜ");
		break;
	case 2:
		printf("Weapon : ⓢ");
		break;
	case 3:
		printf("Weapon : ⓛ");
		break;
	case 4:
		printf("Weapon : ⓑ");
		break;
	}
}
void show_bullet() {
	gotoxy(63, 14);
	switch (weapon_kind) {
	case 0:
		printf("Bullet : ∞");
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
//아이템 구현
//아이템 생성
void show_item() {
	int x, y;
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (weapon[x][y] != 0) {
				switch (weapon[x][y]) {
				case 1:
					gotoxy(x, y);
					printf("ⓜ"); //머신건 
					break;
				case 2:
					gotoxy(x, y);
					printf("ⓢ"); //샷건
					//bullet[2]=?
					break;
				case 3:
					gotoxy(x, y);
					printf("ⓛ");
					//bullet[3]=?
					break;
				}
			}
		}
	}
}
int what_weapon = 1; // 동영상 찍는 용도
void spawn_weapon() {
	int x, y;
	x = (rand() % 58) / 2 * 2;
	y = rand() % (HEIGHT - 1) + 1;  // 제일 상단은 피한다
	//what_weapon = rand() % 3 + 1;// 1, 2, 3 : 총 종류
	//what_weapon = 3;
	//아이템은 2개까지만 생성
	if (weapon_num < 2) {
		// 좀비가 없고 무기가 없고 플레이어 없는 곳에 생성
		if (zombie[x][y] == 0 && x != oldx && y != oldy && weapon[x][y] == 0) {
			switch (what_weapon) {
			case 1:
				//gotoxy(x, y);
				//printf("ⓜ"); //머신건 
				weapon[x][y] = 1;
				what_weapon++;
				break;
			case 2:
				//gotoxy(x, y);
				//printf("ⓢ"); //샷건
				weapon[x][y] = 2;
				what_weapon++;
				//bullet[2]=?
				break;
			case 3:
				/*gotoxy(x, y);
				printf("ⓛ");*/
				weapon[x][y] = 3;
				what_weapon = 1;
				//bullet[3]=?
				break;
			}
			weapon_num++;
		}
	}
}
//아이템 장착
void equip_weapon() {
	// 무기 먹으면 없어짐
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
//총쏘기
struct gun {
	int x, y;
	int count;
	int zombie_newx;
	int zombie_newy;
	char direction;
};
struct gun A;// 권총
struct gun B[30];// 기관총
struct gun C[15];// 샷건
struct gun D[5];// 레이저
struct gun E[10];// 좀비가 날리는 총
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
	if (A.x != 0 && A.y != 0) {//A.x, A.y가 0 0 일때만 총이 나가도록
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
		//-1일땐 총알을 쓴거임
		if (B[i].x != -1 && B[i].x != 0 && B[i].y != 0) {
			switch (B[i].direction) {
			case UP2: //이거 구현 지렸다 소감때 쓰기
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
						B[i].x = -1; // -1 이면 총알을 쓴거임
						show_bullet();
						if (bullet[1] == 0 && B[29].x == -1) {// 총알 다 쓰면 권총으로
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
						B[i].x = -1; // -1 이면 총알을 쓴거임
						if (bullet[1] == 0 && B[29].x == -1) {// 총알 다 쓰면 권총으로
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
						B[i].x = -1; // -1 이면 총알을 쓴거임
						if (bullet[1] == 0 && B[29].x == -1) {// 총알 다 쓰면 권총으로
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
						B[i].x = -1; // -1 이면 총알을 쓴거임
						if (bullet[1] == 0 && B[29].x == -1) {// 총알 다 쓰면 권총으로
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
			case UP2: //이거 구현 지렸다 소감때 쓰기
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
						C[i].x = -1; // -1 이면 총알을 쓴거임
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
						C[i].x = -1; // -1 이면 총알을 쓴거임
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
						C[i].x = -1; // -1 이면 총알을 쓴거임
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
						C[i].x = -1; // -1 이면 총알을 쓴거임
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
//좀비 구현 (연결리스트로 했으면 더 좋았을듯 :x,y life 점수 이렇게 element
//질문. 좀비들끼리 안겹치게 하려고 했는데 뭔가 이상합니다
void init_gun_zombie(int x, int y) { //좀비3총알 초기화
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
void produce_zombie() //좀비 생성
{
	int x, y, r;
	x = (rand() % 56) / 2 * 2 + 2;//생성은 56까지
	y = rand() % (HEIGHT - 1) + 1;  // 제일 상단은 피한다
	r = rand() % 100; //좀비가 나올 확률
	//총쏘는 좀비는 10마리 이하
	check_e = 0;
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (zombie[i][j] == 3)
				check_e++;
		}
	}
	//좀비 수는 zombie_count 이하
	if (zombie_count <= restrict_zombie) {//restrict_zombie = 좀비가 나올 수 있는 수
		if (r > zombie_1_P) { //일반 좀비 50퍼센트 확률로 나옴
			if (zombie[x][y] == 0) {
				zombie[x][y] = 1;
				zombie_count++;
			}
		}
		else if (r > zombie_2_P) { // 블루 좀비 25퍼센트 확률로 나옴
			if (zombie[x][y] == 0) {
				zombie[x][y] = 2;
				zombie_count++;
			}
		}
		else if (check_e < 10) {  //레드 좀비 25퍼센트 확률로 나옴, 대신 10마리 까지만 나온다
			if (zombie[x][y] == 0) {
				zombie[x][y] = 3;
				zombie_count++;
				init_gun_zombie(x, y);
			}
		}
		textcolor(BLACK, WHITE);
	}
}
// 좀비 1번 함수들
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
void move_zombie_1() {// 1번 좀비 움직임 함수// 문제는 move_zombie_1에 있음~
	int x, y;
	int zombie_newx = 0;
	int zombie_newy = 0;
	int newzombie[60][HEIGHT] = { 0 };
	if (zombie_count == 0)
		return;
	erase_zombie_1(); // 일단 좀비 1 다 지움
	for (x = 0; x < 60; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 1) {//1번 좀비일때
				if ((oldx - x) > 0) { //따라오게 새로운 좌표 만든다
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
				zombie[x][y] = 0;//기존에 있던 좀비 없애기
				if (zombie_newx >= 0 && zombie_newx < 60 && zombie_newy >= 0 && zombie_newy < HEIGHT) {
					if (zombie[zombie_newx][zombie_newy] == 0 && newzombie[zombie_newx][zombie_newy] == 0) {
						// 새로운 위치에 좀비가 없으면 좀비 이동
						newzombie[zombie_newx][zombie_newy] = 1;
					}
					else {
						// 새로운 위치에 이미 좀비가 있으면 이전 위치로 유지
						newzombie[x][y] = 1;
					}
				}
				else {
					// 좀비가 범위를 벗어나면 이전 위치로 유지
					newzombie[x][y] = 1;
				}
			}
		}
	}
	for (x = 0; x < 60; x++) {//newzombie의 값을 넘겨줌 위에서 바로 zombie한테 주면
		//중복되기 때문에 따로 넣고 넣어줘야함
		for (y = 0; y < HEIGHT; y++) {
			if (newzombie[x][y] == 1) {
				zombie[x][y] = newzombie[x][y];
				newzombie[x][y] = 0;
			}
		}
	}
	show_zombie_1();//옮겨진 좌표들의 좀비 1 출력
}
// 좀비 2번 함수들
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
void move_zombie_2() {// 2번 좀비 움직임 함수
	int x, y;
	int zombie_newx = 0;
	int zombie_newy = 0;
	int newzombie[60][HEIGHT] = { 0 };
	if (zombie_count == 0)
		return;
	erase_zombie_2(); // 일단 좀비 2 다 지움
	for (x = 0; x < 60; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (zombie[x][y] == 2) {//2번 좀비일때
				if ((oldx - x) > 0) { //따라오게 새로운 좌표 만든다
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
				zombie[x][y] = 0;//기존에 있던 좀비 없애기
				if (zombie_newx >= 0 && zombie_newx < 60 && zombie_newy >= 0 && zombie_newy < HEIGHT) {
					if (zombie[zombie_newx][zombie_newy] == 0 && newzombie[zombie_newx][zombie_newy] == 0) {
						// 새로운 위치에 좀비가 없으면 좀비 이동
						newzombie[zombie_newx][zombie_newy] = 2;
					}
					else {
						// 새로운 위치에 이미 좀비가 있으면 이전 위치로 유지
						newzombie[x][y] = 2;
					}
				}
				else {
					// 좀비가 범위를 벗어나면 이전 위치로 유지
					newzombie[x][y] = 2;
				}
			}
		}
	}
	for (x = 0; x < 60; x++) {//newzombie의 값을 넘겨줌 위에서 바로 zombie한테 주면
		//중복되기 때문에 따로 넣고 넣어줘야함
		for (y = 0; y < HEIGHT; y++) {
			if (newzombie[x][y] == 2) {
				zombie[x][y] = newzombie[x][y];
				newzombie[x][y] = 0;
			}
		}
	}
	show_zombie_2();//옮겨진 좌표들의 좀비 2 출력
}
// 좀비 3번 함수들
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
void move_zombie_3() {// 3번 좀비 총알 움직임 함수
	for (int i = 0; i < 10; i++) {
		if (E[i].zombie_newx != 0 && E[i].zombie_newy != 0) {//따라오게 설정
			erase_bullet_2(E[i]);
			show_zombie_3(); //좀비 3은 가만히 있는다
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
			if (E[i].count == 7 && zombie[E[i].x][E[i].y] != 3) { //7번 움직이면 총알 사라짐
				erase_bullet_2(E[i]);
				E[i].zombie_newx = 0;
				E[i].zombie_newy = 0;
			}
		}
	}
}
//총알에 맞으면 좀비 죽음 구현
void die_zombie() {
	// 총알이 있는 곳에 좀비가 있음
	   //여기서도 switch해서 erase_bullet_1 이런식으로 해야할듯
	erase_bullet(A);
	zombie_count--;
	switch (zombie[A.x][A.y]) { // 죽은 좀비에 따라 점수 증가
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
	zombie[A.x][A.y] = 0; //좀비 죽음
	A.x = 0; //다시 총알이 나갈 수 있도록 0,0 으로 초기화
	A.y = 0;
}
void die_zombie_1(int n) {
	erase_bullet(B[n]);
	zombie_count--;
	switch (zombie[B[n].x][B[n].y]) { // 죽은 좀비에 따라 점수 증가
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
	B[n].x = -1; // 좀비 맞으면 총알을 쓴거임
}
void die_zombie_2(int n) {
	erase_bullet(C[n]);
	zombie_count--;
	switch (zombie[C[n].x][C[n].y]) { // 죽은 좀비에 따라 점수 증가
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
	C[n].x = -1; // 좀비 맞으면 총알을 쓴거임
}
// 좀비랑 부딪히면 라이프 깎임 함수
void die_player() {
	//부딪히면 좀비 죽음
	zombie[oldx][oldy] = 0;
	zombie_count--;
	life--; //라이프 깎임
	show_life();
}
//플레이어 구현
void die_player_2(int n) {
	erase_bullet_2(E[n]);
	//좀비 3이 다시 쏠 수 있게 초기화
	E[n].zombie_newx = E[n].x;
	E[n].zombie_newy = E[n].y;
	life--;
	show_life();
}
void control_game() {//난이도 조절 : 좀비 속도 증가
	restrict_zombie += 2;
	zombie1_frame_sync -= 1;
	zombie2_frame_sync -= 1;
	zombie3_frame_sync -= 1;
	stage++;
}
//void control_game2() {//난이도 조절 : 좀비 수 증가, 나오는 확률 변경
//	restrict_zombie += 3;
//	zombie_1_P += 5;
//	zombie_2_P += 5;
//}
void player(unsigned char ch)
{
	int move_flag = 0;
	static unsigned char last_ch = 0;

	if (called[0] == 0) { // 처음 또는 Restart
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
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		putstar(newx, newy, PLAYER); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
	}
	//Sleep(Delay);
}
//게임 초기화
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
//버그찾기
// 해야할거 샷건 총알 다 쓰면 안바뀌는듯
// 총알 다 쓰면 중간에 멈추는거
// 난이도 올리기
void main(int argc, char* argv[])
{
	unsigned char ch;
	static char last_ch;
	int keep_moving;
	int frame_count = 0;
	srand(time(NULL));
	draw_box2(0, 0, 78, 23, "★");
	PlaySound(TEXT("wave_file.wav"), 0, SND_FILENAME | SND_ASYNC);
	//if (argc == 1) {
	//	// bgm 재생
	//	PlaySound(TEXT("bgm.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//	// 게임 실행하고 게임이 종료될때까지 기다린다.
	//	spawnl(P_WAIT, argv[0], argv[0], "gamerun", NULL);
	//	// 게임이 종료되면 bgm process도 종료
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
		//기본총으로 좀비 죽이기
		if (zombie[A.x][A.y] != 0) { //총알이랑 좀비 좌표가 같으면 좀비 죽음)
			die_zombie();
		}
		//기관총으로 좀비 죽이기
		for (int i = 0; i < 30; i++) {
			if (zombie[B[i].x][B[i].y] != 0) { //총알이랑 좀비 좌표가 같으면 좀비 죽음)
				die_zombie_1(i);
			}
		}
		//샷건으로 좀비 죽이기
		for (int i = 0; i < 15; i = i + 3) {
			if (zombie[C[i].x][C[i].y] != 0) { //총알이랑 좀비 좌표가 같으면 좀비 죽음
				die_zombie_2(i);
			}
			else if (zombie[C[i + 1].x][C[i + 1].y] != 0) {
				die_zombie_2(i + 1);
			}
			else if (zombie[C[i + 2].x][C[i + 2].y] != 0) {
				die_zombie_2(i + 2);
			}
		}
		//레이저로 좀비 죽이기는 move_bullet_3에 포함됨

		if (zombie[oldx][oldy] != 0) {//좌표랑 좀비랑 같으면 라이프 깎임
			die_player();
			PlaySound(TEXT("damage.wav"), 0, SND_FILENAME | SND_ASYNC);
			if (life == 0)
				goto END;
		}
		//좀비 3의 투사체에 맞으면 라이프 깎임
		for (int i = 0; i < 10; i++) {
			if (E[i].zombie_newx == oldx && E[i].zombie_newy == oldy) {
				die_player_2(i);
				PlaySound(TEXT("damage.wav"), 0, SND_FILENAME | SND_ASYNC);
				if (life == 0)
					goto END;
			}
		}
		if (weapon[oldx][oldy] != 0) {//좌표랑 아이템 같으면 무기 장착
			equip_weapon();
		}
		if (frame_count % zombie_appear == 0) { //zombie_appear초 마다 좀비 생성?
			produce_zombie();
		}
		if (kbhit() == 1) {  // 키보드가 눌려져 있으면
			ch = getch(); // key 값을 읽는다
			// ESC 누르면 프로그램 종료
			if (ch == ESC) {
				gotoxy(25, 11);
				exit(0);
				break;
			}
			if (ch == SPECIAL1) { // 만약 특수키
				// 예를 들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
				ch = getch();
				// Player는 방향키로 움직인다.
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player(ch);
					//player(last_ch);
					break;
				default: // 방향 전환이 아니면
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
			//switch로 총 종류를 나눈다.
			switch (weapon_kind) {
			case 0:
				move_bullet(); // 총알은 player()와는 별도로 이동시킨다.
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
			move_zombie_1(); //zombie1의 위치를 변경한다.
		if (frame_count % zombie2_frame_sync == 0)
			move_zombie_2(); //zombie2의 위치를 변경한다.
		if (frame_count % zombie3_frame_sync == 0)
			move_zombie_3(); //zombie3이 시간 마다 투사체를 던진다.
		if (frame_count % weapon_frame_sync == 0)
			spawn_weapon();

		Sleep(Delay); // Delay 값을 줄이고
		frame_count++; // frame_count 값으로 속도 조절을 한다.
		//일정 점수가 넘어가면 프레임 값 어려워지도록 조절****************)
		if (score / 700 > stage)// 점수가 500점 마다 좀비 스피드를 올린다
			control_game();
		//if (score / 500 > stage2)// 점수가 500점 마다 좀비 수를 늘리고 확률을 변경한다
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
