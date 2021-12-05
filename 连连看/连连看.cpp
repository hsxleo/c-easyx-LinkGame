#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <comdef.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")//音频

//菜单窗口的高和宽
#define MENU_HEIGHT 600   
#define MENU_WIDTH 600  

//游戏窗口的高和宽
#define GAME_HEIGHT 600
#define GAME_WIDTH 1000

//图片变量
IMAGE menuBackground;  //菜单背景图
IMAGE gameBackground;  //游戏背景图
IMAGE cardimage[20];  //卡牌图片


int card[20][20];  //存放10*15张卡牌图片下标的数组
int tmp[20][20] = { 0 };  //用来记录10对相邻的card    0为没有card 1为普通的card  2为那10对相邻的card 


//选择的两张图片
struct axis {
	int x;
	int y;
};
struct axis choose[2];  //选择的两张图的下标 

struct axis hint1, hint2;  //提示的两张图


//用户 相关变量
struct user {
	char name[20];  //玩家名
	int score;  //玩家积分
};
struct user users[100];  //用户们
int len = 0;   //用户个数

//游戏分数 和 时间
int score = 0;
int score2 = 0;
time_t start = 0, end = 0, time1, time2;

//洗牌次数
int shuffle_num = 1;
//提示次数
int hint_num = 2;


void loadResource();//加载资源
void drawMenu();//绘制菜单
void mouseMenu();//菜单鼠标交互
void drawGame();//绘制游戏
void randCard();   //随机生成卡牌

void playGame();//玩游戏
bool horizon(struct axis a, struct axis b);//水平消除
bool vertical(struct axis a, struct axis b);//垂直消除
bool turn(struct axis a, struct axis b);//转弯  (1次和2次)

void shuffle();//洗牌
bool hint(); //提示

void readFile();  //读取文件
void saveFile();  //写入文件
void setGame();  //游戏设置
void rankList();  //排行榜


//加载资源
void loadResource() {
	loadimage(&menuBackground, "image\\menubackground.jpg", MENU_WIDTH, MENU_HEIGHT);
	loadimage(&gameBackground, "image\\gamebackground.jpg", GAME_WIDTH, GAME_HEIGHT);
	int i;
	char str[20];
	for (i = 1; i <= 13; i++) {   //将13张卡牌的图片加载进来
		wsprintf(str, "image\\%d.png", i);   //生成图片地址字符串
		loadimage(&cardimage[i], str, 30, 30);
	}
}

//绘制菜单
void drawMenu() {
	//生成画布
	initgraph(MENU_WIDTH, MENU_HEIGHT);
	//菜单背景图片
	putimage(0, 0, &menuBackground);

	//绘制选项   开始游戏  排行榜
	settextcolor(WHITE);          //设置字体颜色
	settextstyle(40, 0, "华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);       //文字背景透明
	outtextxy(200, 120, "开始游戏");
	outtextxy(200, 190, "游戏设置");
	outtextxy(220, 260, "排行榜");
	outtextxy(200, 330, "退出游戏");


}

//菜单鼠标交互
void mouseMenu() {

	drawMenu();      //绘制菜单界面
	MOUSEMSG m;  //鼠标变量   用来获取鼠标信息

	while (true) {
		//获取鼠标位置
		m = GetMouseMsg();

		//判断鼠标位置
		if (m.x >= 200 && m.x <= 340 && m.y >= 120 && m.y <= 160) {  //鼠标在 开始游戏 的位置
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				playGame();  //开始游戏
			}
		}
		else if (m.x >= 200 && m.x <= 340 && m.y >= 190 && m.y <= 230) {  //鼠标在 游戏设置 的位置
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				setGame();  //游戏设置

			}
		}
		else if (m.x >= 220 && m.x <= 340 && m.y >= 260 && m.y <= 300) {  //鼠标在 排行榜 的位置
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				rankList();  //排行榜
			}
		}
		else if (m.x >= 200 && m.x <= 340 && m.y >= 330 && m.y <= 370) {  //鼠标在 退出游戏 的位置
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				exit(0);  //退出游戏
			}
		}
	}
}

//绘制游戏
void drawGame() {
	//清空画布
	cleardevice();
	//游戏背景图片
	putimage(0, 0, &gameBackground);

	//重新开始  返回菜单
	settextcolor(BLACK);          //设置字体颜色
	settextstyle(30, 0, "华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);       //文字背景透明

	//上方功能栏
	char s[20] = "";
	outtextxy(100, 50, "得分");
	outtextxy(300, 50, "提示");
	outtextxy(500, 50, "洗牌");
	sprintf(s, "%ds", 360 - (end - start));
	outtextxy(700, 50, s);
	settextcolor(GREEN);
	sprintf(s, "%d分", score+score2);
	outtextxy(200, 50, s);	
	sprintf(s, "%d次", hint_num);
	outtextxy(400, 50, s);
	sprintf(s, "%d次", shuffle_num);
	outtextxy(600, 50, s);
	settextcolor(RED);
	outtextxy(800, 50, "退出");
	//显示卡牌
	int i, j;
	for (i = 0; i <= 11; i++) {
		for (j = 0; j <= 15; j++) {
			if (card[i][j] != 0) {
				putimage(280 + j * 30, 150 + i * 30, &cardimage[card[i][j]]);
			}
		}
	}
}

//随机生成卡牌
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

		if (tmp[tmp_x1][tmp_y1] == 0 && tmp[tmp_x2][tmp_y2] == 0) {  //两张card都没有放过东西
			tmp[tmp_x1][tmp_y1] = 2;
			tmp[tmp_x2][tmp_y2] = 2;
			int tmp_card = rand() % 13 + 1;  //随机的13张card
			card[tmp_x1][tmp_y1] = tmp_card;
			card[tmp_x2][tmp_y2] = tmp_card;
			tmp_n--;
		}
	}

	int tmp_i = 0, tmp_j = 0;
	tmp_n = (10 * 14 - 10 * 2) / 2;
	while (tmp_n != 0) {   //生成card
		if (tmp[tmp_i][tmp_j] == 0) {
			tmp[tmp_i][tmp_j] = 1;
			int tmp_card = rand() % 13 + 1;  //随机的13张card
			card[tmp_i][tmp_j] = tmp_card;
			while (1) {
				tmp_j++;
				if (tmp_j == 14) {   //满14,j = 0,i+1;
					tmp_i = tmp_i + 1;
					tmp_j = 0;
				}
				if (tmp[tmp_i][tmp_j] == 0) {   //第二个位置判断有没有
					tmp[tmp_i][tmp_j] = 1;
					card[tmp_i][tmp_j] = tmp_card;
					tmp_n--;
					break;
				}
			}
		}
		tmp_j++;
		if (tmp_j == 14) {   //满14,j = 0,i+1;
			tmp_i = tmp_i + 1;
			tmp_j = 0;
		}
	}

	tmp_n = 100;
	while (tmp_n != 0) {   //随意的两两交换打乱顺序     那10对不动
		//随机生成两个位置
		int tmp_x1 = rand() % 10;
		int tmp_y1 = rand() % 14;
		int tmp_x2 = rand() % 10;
		int tmp_y2 = rand() % 14;
		if (tmp[tmp_x1][tmp_y1] != 2 && tmp[tmp_x2][tmp_y2] != 2) {  //交换两张普通生成crad的图片下标
			int t = card[tmp_x1][tmp_y1];
			card[tmp_x1][tmp_y1] = card[tmp_x2][tmp_y2];
			card[tmp_x2][tmp_y2] = t;
			tmp_n--;
		}
	}

	//给card加一圈
	for (i = 10; i > 0; i--) {
		for (j = 14; j > 0; j--) {
			card[i][j] = card[i - 1][j - 1];
		}
	}

	//这一圈都是0
	for (i = 0; i <= 11; i++) {
		for (j = 0; j <= 15; j++) {
			if (i == 0 || j == 0 || i == 11 || j == 15) {
				card[i][j] = 0;
			}
		}
	}
}

//玩游戏
void playGame() {
	//随机生成卡牌
	randCard();

	//生成画布
	initgraph(GAME_WIDTH, GAME_HEIGHT);

	drawGame();   //绘制游戏画面

	int hint_flg = 0;  //是否在提示
	bool win = true;
	bool time_flg = false;
	//计时
	start = time(NULL);  //计时开始

	MOUSEMSG m;  //鼠标变量   用来获取鼠标信息
	int n = 0;    //每两次交换一次
	while (true) {
		//获取鼠标位置
		int i, j;
		if (!MouseHit() && win) {
			if (end - start >= 360 && win) {
				win = false;
				//开始双缓冲绘图
				BeginBatchDraw();
				end = time(NULL);
				drawGame();
				settextcolor(RED);          //设置字体颜色
				settextstyle(50, 0, "华文新魏"); //设置文字样式
				setbkmode(TRANSPARENT);       //文字背景透明
				outtextxy(370, 120, "游戏失败！");
				//结束双缓冲绘图
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
					if (m.uMsg == WM_LBUTTONDOWN && card[i][j] != 0) {   //鼠标按下
						//记住选择的下标
						choose[n].x = i;
						choose[n].y = j;
						n++;
					}
					if (n == 2 && choose[0].x == choose[1].x && choose[0].y == choose[1].y) {   //第二次点的和第一次一样
						n = n % 2;
					}
				}
			}
		}
		if (m.x >= 500 && m.x <= 560 && m.y >= 50 && m.y <= 80) {  //洗牌
			if (m.uMsg == WM_LBUTTONDOWN) {
				if (shuffle_num > 0) {
					hint_flg = 0;
					shuffle_num--;
					shuffle();
				}
			}
		}
		else if (m.x >= 300 && m.x <= 360 && m.y >= 50 && m.y <= 80) {  //提示
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				if (hint_num > 0) {
					hint_flg = 1;
					hint_num--;
					hint();
				}
			}
		}
		else if (m.x >= 800 && m.x <= 860 && m.y >= 50 && m.y <= 80) {  //退出
			if (m.uMsg == WM_LBUTTONDOWN) {   //鼠标按下
				score = 0;
				score2 = 0;
				start = 0;
				end = 0;
				shuffle_num = 1;
				hint_num = 2;
				mouseMenu();
			}
		}
		if (n > 1) {   //选择了两个  不一样的
			n = 0;
			setcolor(RED);
			rectangle(280 + choose[1].y * 30, 150 + choose[1].x * 30, 280 + (choose[1].y + 1) * 30, 150 + (choose[1].x + 1) * 30);
			Sleep(100);  //暂停
			if (card[choose[0].x][choose[0].y] == card[choose[1].x][choose[1].y]) {  //判断是否一样
				if (horizon(choose[0], choose[1]) || vertical(choose[0], choose[1]) || turn(choose[0], choose[1])) {//判断选择的两个是否可以消除
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
			//开始双缓冲绘图
			BeginBatchDraw();
			drawGame();
			settextcolor(RED);          //设置字体颜色
			settextstyle(50, 0, "华文新魏"); //设置文字样式
			setbkmode(TRANSPARENT);       //文字背景透明
			outtextxy(370, 120, "游戏胜利！");
			//结束双缓冲绘图
			EndBatchDraw();
			// 定义字符串缓冲区，并接收用户输入
			char str[11];
			InputBox(str, 10, "请输入昵称:", 0, 0, 0, false);
			//保存成绩到结构体
			int n = strlen(str);
			str[n] = '\0';
			int flg = 0;
			for (i = 0; i < len; i++) {  //判断该用户是否存在
				if (strcmp(str, users[i].name) == 0) {  //若存在
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
			saveFile();  //写入文件
		}
		else if (end - start >= 360 && win) {
			win = false;
			//开始双缓冲绘图
			BeginBatchDraw();
			drawGame();
			settextcolor(RED);          //设置字体颜色
			settextstyle(50, 0, "华文新魏"); //设置文字样式
			setbkmode(TRANSPARENT);       //文字背景透明
			outtextxy(370, 120, "游戏失败！");
			//结束双缓冲绘图
			EndBatchDraw();
		}
		else if (win) {
			end = time(NULL);
			//开始双缓冲绘图
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
			//结束双缓冲绘图
			EndBatchDraw();
		}
		if (!hint() && score < 140) {  //如果不能继续了  自动洗牌
			shuffle();
		}

	}
}

//水平消除
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
//垂直消除
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
//转弯  (1次和2次)
bool turn(struct axis a, struct axis b) {
	int i, j;
	struct axis tmp1, tmp2;
	if (a.x >b.x) {  //a在左边
		struct axis t = a;
		a = b;
		b = t;
	}

	for  (i = 0; i <= 15; i++) {   //先水平再垂直
		tmp1.x = a.x;
		tmp1.y = i;
		tmp2.x = b.x;
		tmp2.y = i;
		if (card[tmp1.x][tmp1.y] != 0 || (card[tmp2.x][tmp2.y] != 0 && b.y != tmp2.y)) {  //拐弯点不为0
			continue;
		}
		if (horizon(a, tmp1) && vertical(tmp1, tmp2) && horizon(tmp2, b)) {
			return true;
		}
	}
	for (i = 0; i <= 11; i++) {   //先垂直再水平
		tmp1.x = i;
		tmp1.y = a.y;
		tmp2.x = i;
		tmp2.y = b.y;
		if (card[tmp1.x][tmp1.y] != 0 || (card[tmp2.x][tmp2.y] != 0 && tmp2.x != b.x)) {  //拐弯点不为0
			continue;
		}
		if (vertical(a, tmp1) && horizon(tmp1, tmp2) && vertical(tmp2, b)) {
			return true;
		}
	}
	return false;
}


//洗牌
void shuffle() {
	srand(unsigned(time(NULL)));
	int i, j;
	int arr[1500] = {0};
	int arr_len = 0;
	//取出来值放到一个数组里面
	for (i = 1; i <= 10; i++) {
		for (j = 1; j <= 14; j++) {
			if (card[i][j] != 0) {
				arr[arr_len++] = card[i][j];
			}
		}
	}
	//对数组里面的元素打乱
	int n = 50;
	while (n--) {
		int t1 = rand() % arr_len;
		int t2 = rand() % arr_len;
		int tmp = arr[t1];
		arr[t1] = arr[t2];
		arr[t2] = tmp;
	}

	//把打乱的数组再放回去
	for (i = 1; i <= 10; i++) {
		for (j = 1; j <= 14; j++) {
			if (card[i][j] != 0) {
				card[i][j] = arr[--arr_len];
			}
		}
	}
}
//提示
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




//读取文件
void readFile() {
	FILE* fp;
	if ((fp = fopen("score.txt", "r+")) == NULL) {   //打开文件   如果没用自动创建
		fp = fopen("score.txt", "w+");
	}
	struct user tmp;
	//将文件内容全部读入用户结构体数组中
	while (fread(&users[len++], sizeof(struct user), 1, fp));
	len--;
	fclose(fp);  //关闭文件
}

//得分写入文件
void saveFile() {
	FILE* fp;
	fp = fopen("score.txt", "w");
	int i;
	for (i = 0; i < len; i++) {
		fwrite(&users[i], sizeof(struct user), 1, fp);
	}
	fclose(fp);
}

//游戏设置
void setGame() {
	cleardevice();
	putimage(0, 0, &menuBackground);
	settextcolor(RED);          //设置字体颜色
	settextstyle(50, 0, "华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);       //文字背景透明
	outtextxy(210, 80, "模式选择");
	settextcolor(BLACK);
	settextstyle(30, 0, "华文新魏"); //设置文字样式
	outtextxy(240, 200, "集中模式");
	outtextxy(400, 500, "返回菜单");
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

//排行榜
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
	//显示前三名
	settextcolor(RED);          //设置字体颜色
	settextstyle(50, 0, "华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);       //文字背景透明
	outtextxy(230, 20, "排行榜");
	settextcolor(BLACK);
	settextstyle(30, 0, "华文新魏"); //设置文字样式
	outtextxy(100, 100, "名次");
	outtextxy(250, 100, "昵称");
	outtextxy(400, 100, "积分");
	outtextxy(100, 200, "状元");
	outtextxy(100, 300, "榜眼");
	outtextxy(100, 400, "探花");
	outtextxy(400, 500, "返回菜单");
	char s[10];
	if (len >= 3) {
		for (i = 0; i < 3; i++) {
			wsprintf(s, "%s", users[i].name);
			outtextxy(250, 100 * (i + 2), s);
			wsprintf(s, "%d分", users[i].score);
			outtextxy(400, 100 * (i + 2), s);
		}
	}
	else {
		for (i = 0; i < len; i++) {
			wsprintf(s, "%s", users[i].name);
			outtextxy(250, 100 * (i + 2), s);
			wsprintf(s, "%d分", users[i].score);
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




				