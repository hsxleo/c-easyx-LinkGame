#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <comdef.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")//��Ƶ

//�˵����ڵĸߺͿ�
#define MENU_HEIGHT 600   
#define MENU_WIDTH 600  

//��Ϸ���ڵĸߺͿ�
#define GAME_HEIGHT 600
#define GAME_WIDTH 1000

//ͼƬ����
IMAGE menuBackground;  //�˵�����ͼ
IMAGE gameBackground;  //��Ϸ����ͼ
IMAGE cardimage[20];  //����ͼƬ


int card[20][20];  //���10*15�ſ���ͼƬ�±������
int tmp[20][20] = { 0 };  //������¼10�����ڵ�card    0Ϊû��card 1Ϊ��ͨ��card  2Ϊ��10�����ڵ�card 


//ѡ�������ͼƬ
struct axis {
	int x;
	int y;
};
struct axis choose[2];  //ѡ�������ͼ���±� 

struct axis hint1, hint2;  //��ʾ������ͼ


//�û� ��ر���
struct user {
	char name[20];  //�����
	int score;  //��һ���
};
struct user users[100];  //�û���
int len = 0;   //�û�����

//��Ϸ���� �� ʱ��
int score = 0;
int score2 = 0;
time_t start = 0, end = 0, time1, time2;

//ϴ�ƴ���
int shuffle_num = 1;
//��ʾ����
int hint_num = 2;


void loadResource();//������Դ
void drawMenu();//���Ʋ˵�
void mouseMenu();//�˵���꽻��
void drawGame();//������Ϸ
void randCard();   //������ɿ���

void playGame();//����Ϸ
bool horizon(struct axis a, struct axis b);//ˮƽ����
bool vertical(struct axis a, struct axis b);//��ֱ����
bool turn(struct axis a, struct axis b);//ת��  (1�κ�2��)

void shuffle();//ϴ��
bool hint(); //��ʾ

void readFile();  //��ȡ�ļ�
void saveFile();  //д���ļ�
void setGame();  //��Ϸ����
void rankList();  //���а�


//������Դ
void loadResource() {
	loadimage(&menuBackground, "image\\menubackground.jpg", MENU_WIDTH, MENU_HEIGHT);
	loadimage(&gameBackground, "image\\gamebackground.jpg", GAME_WIDTH, GAME_HEIGHT);
	int i;
	char str[20];
	for (i = 1; i <= 13; i++) {   //��13�ſ��Ƶ�ͼƬ���ؽ���
		wsprintf(str, "image\\%d.png", i);   //����ͼƬ��ַ�ַ���
		loadimage(&cardimage[i], str, 30, 30);
	}
}

//���Ʋ˵�
void drawMenu() {
	//���ɻ���
	initgraph(MENU_WIDTH, MENU_HEIGHT);
	//�˵�����ͼƬ
	putimage(0, 0, &menuBackground);

	//����ѡ��   ��ʼ��Ϸ  ���а�
	settextcolor(WHITE);          //����������ɫ
	settextstyle(40, 0, "������κ"); //����������ʽ
	setbkmode(TRANSPARENT);       //���ֱ���͸��
	outtextxy(200, 120, "��ʼ��Ϸ");
	outtextxy(200, 190, "��Ϸ����");
	outtextxy(220, 260, "���а�");
	outtextxy(200, 330, "�˳���Ϸ");


}

//�˵���꽻��
void mouseMenu() {

	drawMenu();      //���Ʋ˵�����
	MOUSEMSG m;  //������   ������ȡ�����Ϣ

	while (true) {
		//��ȡ���λ��
		m = GetMouseMsg();

		//�ж����λ��
		if (m.x >= 200 && m.x <= 340 && m.y >= 120 && m.y <= 160) {  //����� ��ʼ��Ϸ ��λ��
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				playGame();  //��ʼ��Ϸ
			}
		}
		else if (m.x >= 200 && m.x <= 340 && m.y >= 190 && m.y <= 230) {  //����� ��Ϸ���� ��λ��
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				setGame();  //��Ϸ����

			}
		}
		else if (m.x >= 220 && m.x <= 340 && m.y >= 260 && m.y <= 300) {  //����� ���а� ��λ��
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				rankList();  //���а�
			}
		}
		else if (m.x >= 200 && m.x <= 340 && m.y >= 330 && m.y <= 370) {  //����� �˳���Ϸ ��λ��
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				exit(0);  //�˳���Ϸ
			}
		}
	}
}

//������Ϸ
void drawGame() {
	//��ջ���
	cleardevice();
	//��Ϸ����ͼƬ
	putimage(0, 0, &gameBackground);

	//���¿�ʼ  ���ز˵�
	settextcolor(BLACK);          //����������ɫ
	settextstyle(30, 0, "������κ"); //����������ʽ
	setbkmode(TRANSPARENT);       //���ֱ���͸��

	//�Ϸ�������
	char s[20] = "";
	outtextxy(100, 50, "�÷�");
	outtextxy(300, 50, "��ʾ");
	outtextxy(500, 50, "ϴ��");
	sprintf(s, "%ds", 360 - (end - start));
	outtextxy(700, 50, s);
	settextcolor(GREEN);
	sprintf(s, "%d��", score+score2);
	outtextxy(200, 50, s);	
	sprintf(s, "%d��", hint_num);
	outtextxy(400, 50, s);
	sprintf(s, "%d��", shuffle_num);
	outtextxy(600, 50, s);
	settextcolor(RED);
	outtextxy(800, 50, "�˳�");
	//��ʾ����
	int i, j;
	for (i = 0; i <= 11; i++) {
		for (j = 0; j <= 15; j++) {
			if (card[i][j] != 0) {
				putimage(280 + j * 30, 150 + i * 30, &cardimage[card[i][j]]);
			}
		}
	}
}

//������ɿ���
void randCard() {
	srand((unsigned)time(NULL));

	int i, j;
	for (i = 0; i < 20; i++) {
		for (j = 0; j < 20; j++) {
			tmp[i][j] = 0;
		}
	}
	int tmp_n = 10;
	int dir[][2] = { 0,1,0,-1,1,0,-1,0 };
	while (tmp_n != 0) {
		int tmp_x1 = rand() % 8 + 1;
		int tmp_y1 = rand() % 12 + 1;
		int tmp_dir = rand() % 4;
		int tmp_x2 = tmp_x1 + dir[tmp_dir][0];
		int tmp_y2 = tmp_y1 + dir[tmp_dir][1];

		if (tmp[tmp_x1][tmp_y1] == 0 && tmp[tmp_x2][tmp_y2] == 0) {  //����card��û�зŹ�����
			tmp[tmp_x1][tmp_y1] = 2;
			tmp[tmp_x2][tmp_y2] = 2;
			int tmp_card = rand() % 13 + 1;  //�����13��card
			card[tmp_x1][tmp_y1] = tmp_card;
			card[tmp_x2][tmp_y2] = tmp_card;
			tmp_n--;
		}
	}

	int tmp_i = 0, tmp_j = 0;
	tmp_n = (10 * 14 - 10 * 2) / 2;
	while (tmp_n != 0) {   //����card
		if (tmp[tmp_i][tmp_j] == 0) {
			tmp[tmp_i][tmp_j] = 1;
			int tmp_card = rand() % 13 + 1;  //�����13��card
			card[tmp_i][tmp_j] = tmp_card;
			while (1) {
				tmp_j++;
				if (tmp_j == 14) {   //��14,j = 0,i+1;
					tmp_i = tmp_i + 1;
					tmp_j = 0;
				}
				if (tmp[tmp_i][tmp_j] == 0) {   //�ڶ���λ���ж���û��
					tmp[tmp_i][tmp_j] = 1;
					card[tmp_i][tmp_j] = tmp_card;
					tmp_n--;
					break;
				}
			}
		}
		tmp_j++;
		if (tmp_j == 14) {   //��14,j = 0,i+1;
			tmp_i = tmp_i + 1;
			tmp_j = 0;
		}
	}

	tmp_n = 100;
	while (tmp_n != 0) {   //�����������������˳��     ��10�Բ���
		//�����������λ��
		int tmp_x1 = rand() % 10;
		int tmp_y1 = rand() % 14;
		int tmp_x2 = rand() % 10;
		int tmp_y2 = rand() % 14;
		if (tmp[tmp_x1][tmp_y1] != 2 && tmp[tmp_x2][tmp_y2] != 2) {  //����������ͨ����crad��ͼƬ�±�
			int t = card[tmp_x1][tmp_y1];
			card[tmp_x1][tmp_y1] = card[tmp_x2][tmp_y2];
			card[tmp_x2][tmp_y2] = t;
			tmp_n--;
		}
	}

	//��card��һȦ
	for (i = 10; i > 0; i--) {
		for (j = 14; j > 0; j--) {
			card[i][j] = card[i - 1][j - 1];
		}
	}

	//��һȦ����0
	for (i = 0; i <= 11; i++) {
		for (j = 0; j <= 15; j++) {
			if (i == 0 || j == 0 || i == 11 || j == 15) {
				card[i][j] = 0;
			}
		}
	}
}

//����Ϸ
void playGame() {
	//������ɿ���
	randCard();

	//���ɻ���
	initgraph(GAME_WIDTH, GAME_HEIGHT);

	drawGame();   //������Ϸ����

	int hint_flg = 0;  //�Ƿ�����ʾ
	bool win = true;
	bool time_flg = false;
	//��ʱ
	start = time(NULL);  //��ʱ��ʼ

	MOUSEMSG m;  //������   ������ȡ�����Ϣ
	int n = 0;    //ÿ���ν���һ��
	while (true) {
		//��ȡ���λ��
		int i, j;
		if (!MouseHit() && win) {
			if (end - start >= 360 && win) {
				win = false;
				//��ʼ˫�����ͼ
				BeginBatchDraw();
				end = time(NULL);
				drawGame();
				settextcolor(RED);          //����������ɫ
				settextstyle(50, 0, "������κ"); //����������ʽ
				setbkmode(TRANSPARENT);       //���ֱ���͸��
				outtextxy(370, 120, "��Ϸʧ�ܣ�");
				//����˫�����ͼ
				EndBatchDraw();
				continue;
			}
			BeginBatchDraw();
			end = time(NULL);
			drawGame();
			if (hint_flg == 1) {
				setcolor(GREEN);
				rectangle(280 + hint1.y * 30, 150 + hint1.x * 30, 280 + (hint1.y + 1) * 30, 150 + (hint1.x + 1) * 30);
				rectangle(280 + hint2.y * 30, 150 + hint2.x * 30, 280 + (hint2.y + 1) * 30, 150 + (hint2.x + 1) * 30);
			}
			if (n > 0) {
				setcolor(RED);
				rectangle(280 + choose[0].y * 30, 150 + choose[0].x * 30, 280 + (choose[0].y + 1) * 30, 150 + (choose[0].x + 1) * 30);
			}
			
			EndBatchDraw();
			continue;
		}
		m = GetMouseMsg();
		for (i = 1; i <= 10; i++) {
			for (j = 1; j <= 14; j++) {
				if (m.x >= 280 + j * 30 && m.x <= 280 + (j + 1) * 30 &&
					m.y >= 150 + i * 30 && m.y <= 150 + (i + 1) * 30) {
					if (m.uMsg == WM_LBUTTONDOWN && card[i][j] != 0) {   //��갴��
						//��סѡ����±�
						choose[n].x = i;
						choose[n].y = j;
						n++;
					}
					if (n == 2 && choose[0].x == choose[1].x && choose[0].y == choose[1].y) {   //�ڶ��ε�ĺ͵�һ��һ��
						n = n % 2;
					}
				}
			}
		}
		if (m.x >= 500 && m.x <= 560 && m.y >= 50 && m.y <= 80) {  //ϴ��
			if (m.uMsg == WM_LBUTTONDOWN) {
				if (shuffle_num > 0) {
					hint_flg = 0;
					shuffle_num--;
					shuffle();
				}
			}
		}
		else if (m.x >= 300 && m.x <= 360 && m.y >= 50 && m.y <= 80) {  //��ʾ
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				if (hint_num > 0) {
					hint_flg = 1;
					hint_num--;
					hint();
				}
			}
		}
		else if (m.x >= 800 && m.x <= 860 && m.y >= 50 && m.y <= 80) {  //�˳�
			if (m.uMsg == WM_LBUTTONDOWN) {   //��갴��
				score = 0;
				score2 = 0;
				start = 0;
				end = 0;
				shuffle_num = 1;
				hint_num = 2;
				mouseMenu();
			}
		}
		if (n > 1) {   //ѡ��������  ��һ����
			n = 0;
			setcolor(RED);
			rectangle(280 + choose[1].y * 30, 150 + choose[1].x * 30, 280 + (choose[1].y + 1) * 30, 150 + (choose[1].x + 1) * 30);
			Sleep(100);  //��ͣ
			if (card[choose[0].x][choose[0].y] == card[choose[1].x][choose[1].y]) {  //�ж��Ƿ�һ��
				if (horizon(choose[0], choose[1]) || vertical(choose[0], choose[1]) || turn(choose[0], choose[1])) {//�ж�ѡ��������Ƿ��������
					hint_flg = 0;
					card[choose[0].x][choose[0].y] = 0;
					card[choose[1].x][choose[1].y] = 0;
					score += 2;
					if (time_flg) {
						time2 = time(NULL);
						if (time2 - time1 < 2) {
							score2 += 2;
						}
						time1 = time2;
					}
					else {
						time1 = time(NULL);
						time_flg = true;
					}
				}
			}
		}
		if (score == 140 && win) {
			win = false;
			//��ʼ˫�����ͼ
			BeginBatchDraw();
			drawGame();
			settextcolor(RED);          //����������ɫ
			settextstyle(50, 0, "������κ"); //����������ʽ
			setbkmode(TRANSPARENT);       //���ֱ���͸��
			outtextxy(370, 120, "��Ϸʤ����");
			//����˫�����ͼ
			EndBatchDraw();
			// �����ַ������������������û�����
			char str[11];
			InputBox(str, 10, "�������ǳ�:", 0, 0, 0, false);
			//����ɼ����ṹ��
			int n = strlen(str);
			str[n] = '\0';
			int flg = 0;
			for (i = 0; i < len; i++) {  //�жϸ��û��Ƿ����
				if (strcmp(str, users[i].name) == 0) {  //������
					flg = 1;
					users[i].score += score + score2;
					break;
				}
			}
			if (flg == 0) {
				strcpy(users[len].name, str);
				users[len].score = score + score2;
				len++;
			}
			saveFile();  //д���ļ�
		}
		else if (end - start >= 360 && win) {
			win = false;
			//��ʼ˫�����ͼ
			BeginBatchDraw();
			drawGame();
			settextcolor(RED);          //����������ɫ
			settextstyle(50, 0, "������κ"); //����������ʽ
			setbkmode(TRANSPARENT);       //���ֱ���͸��
			outtextxy(370, 120, "��Ϸʧ�ܣ�");
			//����˫�����ͼ
			EndBatchDraw();
		}
		else if (win) {
			end = time(NULL);
			//��ʼ˫�����ͼ
			BeginBatchDraw();
			drawGame();
			if (hint_flg == 1) {
				setcolor(GREEN);
				rectangle(280 + hint1.y * 30, 150 + hint1.x * 30, 280 + (hint1.y + 1) * 30, 150 + (hint1.x + 1) * 30);
				rectangle(280 + hint2.y * 30, 150 + hint2.x * 30, 280 + (hint2.y + 1) * 30, 150 + (hint2.x + 1) * 30);
			}
			if (n > 0) {
				setcolor(RED);
				rectangle(280 + choose[0].y * 30, 150 + choose[0].x * 30, 280 + (choose[0].y + 1) * 30, 150 + (choose[0].x + 1) * 30);
			}
			//����˫�����ͼ
			EndBatchDraw();
		}
		if (!hint() && score < 140) {  //������ܼ�����  �Զ�ϴ��
			shuffle();
		}

	}
}

//ˮƽ����
bool horizon(struct axis a,struct axis b) {
	if (a.x != b.x) {
		return false;
	}

	int y1 = a.y;
	int y2 = b.y;
	if (y1 > y2) {
		int t = y1;
		y1 = y2;
		y2 = t;
	}
	int i;
	for (i = y1 + 1; i < y2; i++) {
		if (card[a.x][i] != 0) { 
			return false;
		}
	}
	return true;
}
//��ֱ����
bool vertical(struct axis a, struct axis b) {
	if (a.y != b.y) {
		return false;
	}

	int x1 = a.x;
	int x2 = b.x;
	if (x1 > x2) {
		int t = x1;
		x1 = x2;
		x2 = t;
	}
	int i;
	for (i = x1 + 1; i < x2; i++) {
		if (card[i][a.y] != 0) {
			return false;
		}
	}
	return true;
}
//ת��  (1�κ�2��)
bool turn(struct axis a, struct axis b) {
	int i, j;
	struct axis tmp1, tmp2;
	if (a.x >b.x) {  //a�����
		struct axis t = a;
		a = b;
		b = t;
	}

	for  (i = 0; i <= 15; i++) {   //��ˮƽ�ٴ�ֱ
		tmp1.x = a.x;
		tmp1.y = i;
		tmp2.x = b.x;
		tmp2.y = i;
		if (card[tmp1.x][tmp1.y] != 0 || (card[tmp2.x][tmp2.y] != 0 && b.y != tmp2.y)) {  //����㲻Ϊ0
			continue;
		}
		if (horizon(a, tmp1) && vertical(tmp1, tmp2) && horizon(tmp2, b)) {
			return true;
		}
	}
	for (i = 0; i <= 11; i++) {   //�ȴ�ֱ��ˮƽ
		tmp1.x = i;
		tmp1.y = a.y;
		tmp2.x = i;
		tmp2.y = b.y;
		if (card[tmp1.x][tmp1.y] != 0 || (card[tmp2.x][tmp2.y] != 0 && tmp2.x != b.x)) {  //����㲻Ϊ0
			continue;
		}
		if (vertical(a, tmp1) && horizon(tmp1, tmp2) && vertical(tmp2, b)) {
			return true;
		}
	}
	return false;
}


//ϴ��
void shuffle() {
	srand(unsigned(time(NULL)));
	int i, j;
	int arr[1500] = {0};
	int arr_len = 0;
	//ȡ����ֵ�ŵ�һ����������
	for (i = 1; i <= 10; i++) {
		for (j = 1; j <= 14; j++) {
			if (card[i][j] != 0) {
				arr[arr_len++] = card[i][j];
			}
		}
	}
	//�����������Ԫ�ش���
	int n = 50;
	while (n--) {
		int t1 = rand() % arr_len;
		int t2 = rand() % arr_len;
		int tmp = arr[t1];
		arr[t1] = arr[t2];
		arr[t2] = tmp;
	}

	//�Ѵ��ҵ������ٷŻ�ȥ
	for (i = 1; i <= 10; i++) {
		for (j = 1; j <= 14; j++) {
			if (card[i][j] != 0) {
				card[i][j] = arr[--arr_len];
			}
		}
	}
}
//��ʾ
bool hint() {
	int i, j,ii,jj;
	for (i = 1; i <= 10; i++) {
		for (j = 1; j <= 14; j++) {
			if (card[i][j] != 0) {
				for (ii = 1; ii <= 10; ii++) {
					for (jj = 1; jj <= 14; jj++) {
						hint1.x = i;
						hint1.y = j;
						hint2.x = ii;
						hint2.y = jj;
						if ((i != ii || j != jj)&& card[i][j] == card[ii][jj] && (horizon(hint1, hint2) || vertical(hint1, hint2) || turn(hint1, hint2))) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}




//��ȡ�ļ�
void readFile() {
	FILE* fp;
	if ((fp = fopen("score.txt", "r+")) == NULL) {   //���ļ�   ���û���Զ�����
		fp = fopen("score.txt", "w+");
	}
	struct user tmp;
	//���ļ�����ȫ�������û��ṹ��������
	while (fread(&users[len++], sizeof(struct user), 1, fp));
	len--;
	fclose(fp);  //�ر��ļ�
}

//�÷�д���ļ�
void saveFile() {
	FILE* fp;
	fp = fopen("score.txt", "w");
	int i;
	for (i = 0; i < len; i++) {
		fwrite(&users[i], sizeof(struct user), 1, fp);
	}
	fclose(fp);
}

//��Ϸ����
void setGame() {
	cleardevice();
	putimage(0, 0, &menuBackground);
	settextcolor(RED);          //����������ɫ
	settextstyle(50, 0, "������κ"); //����������ʽ
	setbkmode(TRANSPARENT);       //���ֱ���͸��
	outtextxy(210, 80, "ģʽѡ��");
	settextcolor(BLACK);
	settextstyle(30, 0, "������κ"); //����������ʽ
	outtextxy(240, 200, "����ģʽ");
	outtextxy(400, 500, "���ز˵�");
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		if (m.x >= 400 && m.x <= 520 && m.y >= 500 && m.y <= 530) {
			if (m.uMsg == WM_LBUTTONDOWN) {
				mouseMenu();
			}
		}
	}
}

//���а�
void rankList() {
	int i, j;
	for (i = 0; i < len - 1; i++) {
		for (j = 0; j < len - i - 1; j++) {
			if (users[j].score < users[j + 1].score) {
				struct user t = users[j];
				users[j] = users[j + 1];
				users[j + 1] = t;
			}
		}
	}
	cleardevice();
	putimage(0, 0, &menuBackground);
	//��ʾǰ����
	settextcolor(RED);          //����������ɫ
	settextstyle(50, 0, "������κ"); //����������ʽ
	setbkmode(TRANSPARENT);       //���ֱ���͸��
	outtextxy(230, 20, "���а�");
	settextcolor(BLACK);
	settextstyle(30, 0, "������κ"); //����������ʽ
	outtextxy(100, 100, "����");
	outtextxy(250, 100, "�ǳ�");
	outtextxy(400, 100, "����");
	outtextxy(100, 200, "״Ԫ");
	outtextxy(100, 300, "����");
	outtextxy(100, 400, "̽��");
	outtextxy(400, 500, "���ز˵�");
	char s[10];
	if (len >= 3) {
		for (i = 0; i < 3; i++) {
			wsprintf(s, "%s", users[i].name);
			outtextxy(250, 100 * (i + 2), s);
			wsprintf(s, "%d��", users[i].score);
			outtextxy(400, 100 * (i + 2), s);
		}
	}
	else {
		for (i = 0; i < len; i++) {
			wsprintf(s, "%s", users[i].name);
			outtextxy(250, 100 * (i + 2), s);
			wsprintf(s, "%d��", users[i].score);
			outtextxy(400, 100 * (i + 2), s);
		}
	}
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		if (m.x >= 400 && m.x <= 520 && m.y >= 500 && m.y <= 530) {
			if (m.uMsg == WM_LBUTTONDOWN) {
				mouseMenu();
			}
		}
	}
}

int main() {
	mciSendString("open music.mp3 alias game", nullptr, 0, nullptr);
	mciSendString("play game repeat", nullptr, 0, nullptr);
	loadResource();
	readFile();
	mouseMenu();
}




				