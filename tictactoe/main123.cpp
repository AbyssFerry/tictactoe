#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <iostream>
#include <ctime>

// 实现了void turnChessType(); // 转换当前棋子类型
// 构造了结构体chessPosRowAndCol {int row;int col;}
// 实现了悔棋函数chessUndo() 以及相关依赖

using namespace std;

#define WIDTH_WINDOW 1400 // 界面宽 1400
#define HEIGHT_WINDOW 900 // 界面高 900
#define WIDTH_CHECKERBOARD 840 // 棋盘宽  要与列数能整除
#define HEIGHT_CHECKERBOARD 840 // 棋盘高 要与行数能整除
#define START_CHECKERBOARD_X 500 // 棋盘开始绘制x位置
#define START_CHECKERBOARD_Y 25 // 棋盘开始绘制y位置
#define ROWS_WINDOW 21 // 行数   标准五子棋是15x15
#define COLS_WINDOW 21 // 列数  
#define GAME_FRAME 60 // 帧数
#define WIN_COUNT 5 // 连成多少个就赢
/*-----------------------------------------------------*/

#define TEXT_TITLE 22 // 标题文字占画面高的多少分之一
#define TEXT_PROMPT 30 // 提示文字占画面高的多少分之一
#define TEXT_OTHER 35 // 其他文字占画面高的多少分之一
#define TEXT_ROW 10 // 文本区域行数
#define BUTTON_HIGH 30 // 按钮高占画面高多少
#define BUTTON_WIDE 3// 按钮宽占标题宽多少
/*-----------------------------------------------------*/
#define BACKGROUND_COLLOR RGB(236, 208, 133)
#define BUTTON_COLLOR_ORINGE RGB(128, 128, 128) 
#define BUTTON_COLLOR_ACTIVATE RGB(160, 160, 160)

/*-----------------------------------------------------*/
int startTextX; // 文字开始绘制x位置（在标题输出函数中初始化）
int startTextY; // 文字开始绘制y位置 （在标题输出函数中初始化）
int textHeight; // 每行文字区域高度 （在标题输出函数中初始化）
int textWeight; // 每行文字区域宽度 （也就是标题的宽度）
int textNowRow; // 代表现在文字输出到第几行 （在标题输出函数中初始化）

// 用于储存棋子行列
typedef struct chessPosRowAndCol {
	int row;
	int col;
};

// 用于倒计时
class Time {
public:
	Time(int h = 0, int m = 0, int s = 0) : h(h), m(m), s(s){}
	Time& operator--() {
		if (m == 0 && h != 0) {
			m += 60;
			h--;
		}
		if (s == 0 && m != 0) {
			s += 60;
			m--;
		}
		s--;
		return *this;
	}
	int h, m, s;
};

int chessCount; // 棋子数量用于平局判定
double row_size; // 行大小
double col_size; // 列大小
int mouseX, mouseY;  // 鼠标位置
ExMessage msg; // 记录键盘消息
char nowChessType; // 当前棋的类型 X黑 O白
chessPosRowAndCol lastChessPos; // 记录上一个棋子的位置，用于实现悔棋
bool isClick; // 是否有鼠标点击
bool running; // 游戏是否结束
int gameResult; // 0 平局， 1 O赢， 2 X赢
int blackScore, whiteScore; // 黑棋和白棋得分
Time blackChessCDTime; // 黑棋倒计时
Time whiteChessCDTime; // 白棋倒计时
Time* nowCDTime; // 现在的倒计时
time_t nowTimeSecondCD; // 用于倒计时的时间
LOGFONT gameTextType; // 游戏文本输出样式类型
TCHAR* nowText; // 接受文本样式参数返回内容
DWORD start_time;
DWORD now_time; // 和start_time 实现帧率控制 
/*-----------------------------------------------------*/

// 储存棋子位置的数组
char gameBoard[ROWS_WINDOW][COLS_WINDOW];


int main(); // 主函数
void init(); // 初始化
void gamesleep(); // 控制帧率
void gameRand(); // 读取数据
void gameProcess(); // 数据处理
void gameDraw(); // 渲染
void gameRestart(); // 重新开始新的一局
void turnChessType(); // 转换当前棋子类型
/*-----------------------------------------------------*/

bool gameChessOver(int row, int col); // 检测是否棋子连成获胜个数或是棋盘满了平局
bool gameTimeOver(); // 检测倒计时是否结束
int chessSum(int row, int col, int dx, int dy); // 检测是否胜利
bool isOverBorder(int& row, int& y); // 检测棋子是否越界
/*-----------------------------------------------------*/
void chessBoardProcess(); // 棋盘数据处理
void countdownProcess(); // 倒计时处理


/*-----------------------------------------------------*/
int centeringLRText(TCHAR* s); // 返回文字居中所需的x偏移量
int centeringHWText(TCHAR* s); // 返回文字居中所需的y偏移量


void drawData(); // 绘制数据
void drawText(); // 绘制文本
void drawGridding(); // 绘制网格
void drawGriddingPoint(); // 绘制网格上的半点
TCHAR* setGameTextType(const TCHAR content[100] = _T("默认输出"), COLORREF colorRGB = RGB(0, 0, 0), double height = 10); // 设置文本样式参数
void drawTextTitie(); // 绘制标题文字
void drawTextAdmitDefeat(); // 绘制认输按钮文字
void gameAdmitDefeat(); // 认输
void drawTextStepBack(); // 绘制悔棋按钮文字
void chessUndo(); // 悔棋
void drawTextPicture(); // 绘制图片
void drawTextPicture_doubleManPlayChess(int startX, int startY); // 绘制两个人下棋画面
void drawTextBlackTime(); // 绘制黑棋剩余时间
void drawTextWhiteTime(); // 绘制白棋剩余时间
void drawTextScore(); // 绘制比分
void drawTextPrompt(); // 绘制提示文字
void showWinner(); // 绘制输赢画面
void drawCircle(int x, int y, int r); // 绘制棋子画圆
bool drawButton(int x, int y, TCHAR* text); // 绘制按钮， 返回按钮是否被触发
/*-----------------------------------------------------*/




void init()
{
	initgraph(WIDTH_WINDOW, HEIGHT_WINDOW);
	// 计算行列的宽
	row_size = HEIGHT_CHECKERBOARD / ROWS_WINDOW;
	col_size = WIDTH_CHECKERBOARD / COLS_WINDOW;

	for (int i = 0; i < ROWS_WINDOW; i++)
	{
		for (int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}

	// 设置背景填充色
	setbkcolor(COLORREF(BACKGROUND_COLLOR));

	// 刚开是得分都为0
	blackScore = 0;
	whiteScore = 0;

	// 初始化倒计时为5分钟, 并且黑方开始计时
	blackChessCDTime = Time(0, 0, 10);
	whiteChessCDTime = Time(0, 0, 10);
	nowCDTime = &blackChessCDTime;

	// 游戏开始运行
	running = true;

	// 一些初始化
	mouseX = -1;
	mouseY = -1;
	nowChessType = 'X';
	isClick = false;
	start_time = GetTickCount();  // 获取开机到现在的时间
	nowTimeSecondCD = time(0); // 返回1970年1月1日午夜以来经过的时间，以秒为单位。
	chessCount = 0;
	gameResult = -1;



}

void gameRestart() {
	running = true; // 游戏进行设为true

	// 清除棋盘
	for (int i = 0; i < ROWS_WINDOW; i++)
	{
		for (int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}

	// 开始棋为黑
	nowChessType = 'X';

	// 倒计时重新
	blackChessCDTime = Time(0, 5, 0);
	whiteChessCDTime = Time(0, 5, 0);
	nowCDTime = &blackChessCDTime;
}

void turnChessType()
{
	if (nowChessType == 'X') nowChessType = 'O';
	else nowChessType = 'X';

	if (nowCDTime == &blackChessCDTime) nowCDTime = &whiteChessCDTime;
	else nowCDTime = &blackChessCDTime;

}

// 绘制网格
void drawGridding()
{
	COLORREF lineColor = RGB(0, 0, 0); // 黑色
	setlinecolor(lineColor);

	// 设置画线的偏移量
	int dirx = START_CHECKERBOARD_X;
	int diry = START_CHECKERBOARD_Y;
	for (int i = 0; i < HEIGHT_CHECKERBOARD; i += row_size)
	{
		line(dirx, i + diry, WIDTH_CHECKERBOARD + dirx - col_size, i + diry);
	}

	for (int i = 0; i < WIDTH_CHECKERBOARD; i += col_size)
	{
		line(i + dirx, diry, i + dirx, HEIGHT_CHECKERBOARD + diry - row_size);
	}

	drawGriddingPoint(); // 绘制网格上的斑点
}

void drawGriddingPoint() {
	setfillcolor(COLORREF(RGB(0, 0, 0))); // 设置为黑色
	int R = min(col_size, row_size) / 9;
	int Dxy = 5; // 旁边4个点距离中心多少行列

	int middleRow = ROWS_WINDOW / 2;
	int middleCol = COLS_WINDOW / 2;
	int middleX = START_CHECKERBOARD_X + col_size * middleCol;
	int middleY = START_CHECKERBOARD_Y + row_size * middleRow;
	fillcircle(middleX, middleY, R); // 绘制中心点

	int left_x = START_CHECKERBOARD_X + (middleCol - Dxy) * col_size;
	int right_x = START_CHECKERBOARD_X + (middleCol + Dxy) * col_size;
	int top_y = START_CHECKERBOARD_Y + (middleRow - Dxy) * row_size;
	int down_y = START_CHECKERBOARD_Y + (middleRow + Dxy) * row_size;
	fillcircle(left_x, top_y, R);
	fillcircle(left_x, down_y, R);
	fillcircle(right_x, top_y, R);
	fillcircle(right_x, down_y, R);
}

// 画圆
void drawCircle(int x, int y, int r)
{
	fillcircle(x, y, r);
}

bool drawButton(int x, int y, TCHAR* text)
{
	int x1 = x - (textWeight / BUTTON_WIDE - textwidth(text)) / 2;
	int y1 = y - (HEIGHT_WINDOW / BUTTON_HIGH - textheight(text)) / 2;
	int x2 = x + textwidth(text) + (textWeight / BUTTON_WIDE - textwidth(text)) / 2;
	int y2 = y + textheight(text) + (HEIGHT_WINDOW / BUTTON_HIGH - textheight(text)) / 2;

	if ((mouseX >= x1 && mouseX <= x2) && (mouseY >= y1 && mouseY <= y2)) {
		setfillcolor(BUTTON_COLLOR_ACTIVATE); // 鼠标在就绘制激活颜色
		solidrectangle(x1, y1, x2, y2);
		if (isClick) return true; // 如果鼠标点击了就代表按钮触发
	}
	else {
		setfillcolor(BUTTON_COLLOR_ORINGE);
		solidrectangle(x1, y1, x2, y2);
	}

	return false;
}


// 绘制数据
void drawData()
{

	for (int i = 0; i < ROWS_WINDOW; i++)
	{
		for (int j = 0; j < COLS_WINDOW; j++)
		{
			int rowSit = i * row_size + START_CHECKERBOARD_Y;
			int colSit = j * col_size + START_CHECKERBOARD_X;
			int r = min(row_size / 2 - 3, col_size / 2 - 3);

			if (gameBoard[i][j] == 'O')
			{
				setfillcolor(COLORREF(RGB(255, 255, 255))); // 设为白色
				setlinecolor(COLORREF(RGB(0, 0, 0)));
				drawCircle(colSit, rowSit, r);
			}
			else if (gameBoard[i][j] == 'X')
			{
				setfillcolor(COLORREF(RGB(0, 0, 0))); // 设为黑色
				setlinecolor(COLORREF(RGB(255, 255, 255)));
				drawCircle(colSit, rowSit, r);
			}
		}

	}
	setlinecolor(COLORREF(RGB(0, 0, 0))); // 把画线颜色改为默认黑
}

void showWinner()
{
	// 胜利提示  GetHWnd用于调用系统弹窗
	if (gameResult == 0)
	{
		MessageBox(GetHWnd(), _T("玩家平局"), _T("胜负情况"), MB_OK);
	}
	else if (gameResult == 1)
	{
		MessageBox(GetHWnd(), _T("白棋玩家胜利(￣y▽￣)╭ Ohohoho....."), _T("胜负情况"), MB_OK);
		whiteScore++; // 白棋得一分
	}
	else if (gameResult == 2)
	{
		MessageBox(GetHWnd(), _T("黑棋玩家胜利○( ＾皿＾)っHiahiahia…"), _T("胜负情况"), MB_OK);
		blackScore++; // 黑棋得一分
	}

}

// 返回文字居中所需的x偏移量
int centeringLRText(TCHAR* s)
{
	return (textWeight - textwidth(s)) / 2;
}

int centeringHWText(TCHAR* s)
{
	return (textHeight - textheight(s)) / 2;
}

void outTextMiddow(int textNowRow)
{
	static int midowDx;
	static int midowDy;
	midowDx = centeringLRText(nowText); // 居中x偏量返回
	midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

TCHAR* setGameTextType(const TCHAR content[100], COLORREF colorRGB, double height)
{
	settextcolor(colorRGB);  // 黑色

	setbkmode(TRANSPARENT); // 设置文字背景模式为透明

	gettextstyle(&gameTextType);  // 获取当前文字样式

	gameTextType.lfHeight = height;  // 根据比例绘制提示文字的高

	settextstyle(&gameTextType);  // 将更改后的样式给文字

	static TCHAR s[100]; // 储存内容数组，初始化一次就行，以后都只要赋值
	_stprintf_s(s, sizeof(s) / sizeof(TCHAR), _T("%s"), content);
	return s;
}

// 绘制标题文字
void drawTextTitie()
{
	nowText = setGameTextType(L"===来吧开始博弈===", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_TITLE);

	// 通过标题初始化位置以及每行文字高度
	static int firstFlag = true; // 只初始化一次
	if (firstFlag)
	{
		firstFlag = false;  // 只用执行一次设置起始位置
		textWeight = textwidth(nowText);
		startTextX = (START_CHECKERBOARD_X - textWeight) / 2; // 使标题文字位于棋盘左侧空余取余的中间
		startTextY = startTextX; // 目前先决定y坐标起始像素设置为和x坐标起始像素一样
		textHeight = (HEIGHT_WINDOW - 2 * startTextY) / TEXT_ROW; // 设置文字区域每行大小
	}

	// 每次输出到标题都从0行开始
	textNowRow = 0;

	outtextxy(startTextX, startTextY + textNowRow * textHeight, nowText);

}

// 绘制提示文字
void drawTextPrompt()
{
	static TCHAR tempText[100]; // 用于拼接
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("目前的棋子是: %s"), nowChessType == 'O' ? L"白棋" : L"黑棋");
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_PROMPT);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText); // 居中x偏量返回
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);

}

void drawTextPicture_doubleManPlayChess(int startX, int startY) {
	// 左边人头
	int lheadX = 20, lheadY = 20, lheadR = 40;
	circle(startX + lheadX, startY + lheadY, lheadR);
	// 左边人身子
	int lbodyLen = 60;
	line(startX + lheadX, startY + lheadY + lheadR, startX + lheadX, startY + lheadY + lheadR + lbodyLen);
	// 左边人手1
	int lhand1X = 120, lhand1Y = 10, lhandDy = lbodyLen / 2 - 10;
	line(startX + lheadX, startY + lheadY + lheadR + lhandDy, startX + lheadX + lhand1X, startY + lheadY + lheadR + lhandDy + lhand1Y);
	// 左边人手2
	int lhand21X = 50, lhand21Y = 30;
	int lhand22X = 41, lhand22Y = 55;
	line(startX + lheadX, startY + lheadY + lheadR + lhandDy, startX + lheadX + lhand21X, startY + lheadY + lheadR + lhandDy + lhand21Y);
	line(startX + lheadX + lhand21X, startY + lheadY + lheadR + lhandDy + lhand21Y, startX + lhand22X, startY + lhand22Y);
	// 左边腿11
	int lleg11X = 60, lleg11Y = 5;
	line(startX + lheadX, startY + lheadY + lheadR + lbodyLen, startX + lheadX + lleg11X, startY + lheadY + lheadR + lbodyLen + lleg11Y);
	// 左边腿12
	int lleg12X = -5, lleg12Y = 70;
	line(startX + lheadX + lleg11X, startY + lheadY + lheadR + lbodyLen + lleg11Y, startX + lheadX + lleg11X + lleg12X, startY + lheadY + lheadR + lbodyLen + lleg11Y + lleg12Y);
	// 左边腿21
	int lleg21X = 40, lleg21Y = 15;
	line(startX + lheadX, startY + lheadY + lheadR + lbodyLen, startX + lheadX + lleg21X, startY + lheadY + lheadR + lbodyLen + lleg21Y);
	// 左边腿22
	int lleg22X = -5, lleg22Y = 60;
	line(startX + lheadX + lleg21X, startY + lheadY + lheadR + lbodyLen + lleg21Y, startX + lheadX + lleg21X + lleg22X, startY + lheadY + lheadR + lbodyLen + lleg21Y + lleg22Y);

	// 左边人拿着的黑棋
	int lTakeChessX = startX + lheadX + lhand1X, lTakeChessY = startY + lheadY + lheadR + lhandDy + lhand1Y;
	int lTakeChessR = 5;
	setfillcolor(COLORREF(RGB(0, 0, 0)));
	fillcircle(lTakeChessX, lTakeChessY, lTakeChessR);

	// 左边椅子
	int lseatAx = startX + lheadX - 20, lseatAy = startY + lheadY + lheadR + lbodyLen + 5;
	int lseatWide = 40, lseatHigh = 75, lseatDy = 10, lseatDx = lseatDy / 2 * 5;
	/*  图像点如图
		  e ------ f
		-		  -|
	   a ------ d  |
	   |		|  |
	   |		|  |
	   |		|  |
	   |		|  g
	   |		| -
	   b ------ c
	*/
	POINT pts[] = {
		{lseatAx, lseatAy},  // a
		{lseatAx, lseatAy + lseatHigh},  // b
		{lseatAx + lseatWide, lseatAy + lseatHigh}, // c
		{lseatAx + lseatWide, lseatAy}, // d
		{lseatAx, lseatAy},  // a
		{lseatAx + lseatDx, lseatAy - lseatDy},  // e
		{lseatAx + lseatDx + lseatWide, lseatAy - lseatDy},  // f
		{lseatAx + lseatWide, lseatAy}, // d
		{lseatAx + lseatDx + lseatWide, lseatAy - lseatDy},  // f
		{lseatAx + lseatDx + lseatWide, lseatAy - lseatDy + lseatHigh},  // g
		{lseatAx + lseatWide, lseatAy + lseatHigh}, // c
		{lseatAx, lseatAy + lseatHigh},  // b
		{lseatAx, lseatAy},  // a

	};
	polygon(pts, 13);


	// 桌子
	int tableAX = startX + lheadX + 63, tableAY = startY + lheadY + lheadR + lbodyLen + lseatDy / 2;
	int tableHiht = 80, tableWide = 140, tableDy = 40, tableDx = tableDy / 2 * 5;
	/*  图像点如图
		  e -------------- f
		-	        	 - |
	   a ------------- d   |
	   |	       	   |   |
	   |	           |   |
	   |	           |   |
	   |	           |   g
	   |	           | -
	   b ------------- c
	*/
	POINT pts2[] = {
		{tableAX, tableAY},  // a
		{tableAX, tableAY + tableHiht}, // b
		{tableAX + tableWide, tableAY + tableHiht}, // c
		{tableAX + tableWide, tableAY}, // d
		{tableAX, tableAY},  // a
		{tableAX + tableDx, tableAY - tableDy}, // e
		{tableAX + tableDx + tableWide, tableAY - tableDy}, // f
		{tableAX + tableWide, tableAY}, // d
		{tableAX + tableDx + tableWide, tableAY - tableDy}, // f
		{tableAX + tableDx + tableWide, tableAY - tableDy + tableHiht}, // g
		{tableAX + tableWide, tableAY + tableHiht}, // c
		{tableAX, tableAY + tableHiht}, // b
		{tableAX, tableAY}  // a
	};
	polygon(pts2, 13);

	// 画桌子上的线
	int tableLineDx = tableWide / 10, tableLineDy = tableDy / 10;
	for (int i = 1; i < 10; i++) {
		line(tableAX + i * tableLineDx, tableAY, tableAX + tableDx + i * tableLineDx, tableAY - tableDy);
		line(tableAX + i * tableLineDy / 2 * 5, tableAY - i * tableLineDy, tableAX + i * tableLineDy / 2 * 5 + tableWide, tableAY - i * tableLineDy);
	}

	// 右边椅子
	int rseatAX = tableAX + tableWide + 63, rseatAY = tableAY;
	int rseatHigh = 75, rseatWide = 40, rseatDy = 10, rseatDx = rseatDy / 2 * 5;
	/*  图像点如图
		  e ------ f
		-		  -|
	   a ------ d  |
	   |		|  |
	   |		|  |
	   |		|  |
	   |		|  g
	   |		| -
	   b ------ c
	*/
	setfillcolor(COLORREF(RGB(236, 208, 133)));
	POINT pts3[] = {
		{rseatAX + rseatDx + rseatWide, rseatAY - rseatDy + rseatHigh}, // g
		{rseatAX + rseatDx + rseatWide, rseatAY - rseatDy}, // f
		{rseatAX + rseatDx, rseatAY - rseatDy}, // e
		{rseatAX, rseatAY}, // a
		{rseatAX + rseatWide, rseatAY}, // d
		{rseatAX + rseatDx + rseatWide, rseatAY - rseatDy}, // f
		{rseatAX + rseatDx + rseatWide, rseatAY - rseatDy + rseatHigh}, // g
		{rseatAX + rseatWide, rseatAY + rseatHigh}, // c
		{rseatAX + rseatWide, rseatAY}, // d
		{rseatAX, rseatAY}, // a
		{rseatAX, rseatAY + rseatHigh}, // b
		{rseatAX + rseatWide, rseatAY + rseatHigh}, // c
		{rseatAX + rseatDx + rseatWide, rseatAY - rseatDy + rseatHigh}, // g

	};
	fillpolygon(pts3, 13);

	// 右边身子
	int rbodyLen = 60;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide, rseatAY - rseatDy / 2 - rbodyLen);

	// 右边人头
	int rheadR = 40, rheadDx = rseatAX + rseatWide, rheadDy = rseatAY - rseatDy / 2 - rbodyLen - rheadR;
	circle(rheadDx, rheadDy, rheadR);

	// 右边脚11
	int rleg11X = 20, rleg11Y = 18;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide - rleg11X, rseatAY - rseatDy / 2 - rleg11Y);

	// 右边脚12
	int rleg12X = 30, rleg12Y = 85;
	line(rseatAX + rseatWide - rleg11X, rseatAY - rseatDy / 2 - rleg11Y, rseatAX + rseatWide - rleg11X - rleg12X, rseatAY - rseatDy / 2 - rleg11Y + rleg12Y);

	// 右边脚21
	int rleg21X = -15, rleg21Y = 1;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide - rleg12X + rleg21X, rseatAY - rseatDy / 2 + rleg21Y);

	// 右边脚22
	int rleg22X = 5, rleg22Y = 25;
	line(rseatAX + rseatWide - rleg12X + rleg21X, rseatAY - rseatDy / 2 + rleg21Y, rseatAX + rseatWide - rleg12X + rleg21X + rleg22X, rseatAY - rseatDy / 2 + rleg21Y + rleg22Y);

	// 右边手
	int rhandStartX = rseatAX + rseatWide, rhandStartY = rseatAY - rseatDy / 2 - rbodyLen / 2 - 10;
	// 右边手1
	int rhand1X = rseatAX + rseatWide - rleg11X - rleg12X, rhand1Y = rseatAY - rseatDy / 2 - rleg11Y + rleg12Y;
	line(rhandStartX, rhandStartY, rhand1X, rhand1Y);

	// 右边手2
	int rhand2X = rseatAX + rseatDx + rseatWide - 15, rhand2Y = rseatAY - rseatDy + 3;
	line(rhandStartX, rhandStartY, rhand2X, rhand2Y);
}

void drawTextPicture() {
	nowText = setGameTextType(L"", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // 当前行加一
	int midowDx = 0;
	int midowDy = 0; // 图片不需要xy居中

	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
	drawTextPicture_doubleManPlayChess(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy);

	textNowRow += 2; // 为图片留出充足空间
}

void drawTextScore() {
	static TCHAR tempText[100]; // 用于拼接
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("黑:%d  Vs  白:%d"), blackScore, whiteScore);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

void drawTextWhiteTime() {
	static TCHAR tempText[100]; // 用于拼接
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("白 %02d:%02d"), whiteChessCDTime.m, whiteChessCDTime.s);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

void drawTextBlackTime()
{
	static TCHAR tempText[100]; // 用于拼接
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("黑 %02d:%02d"), blackChessCDTime.m, blackChessCDTime.s);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

// 绘制悔棋按钮文字
void drawTextStepBack()
{
	nowText = setGameTextType(L"悔棋", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回

	static int startX = startTextX + midowDx;
	static int startY = startTextY + textNowRow * textHeight + midowDy;

	bool pressButton = drawButton(startX, startY, nowText);
	if (pressButton) chessUndo();
	outtextxy(startX, startY, nowText);
}

void chessUndo()
{
	if (gameBoard[lastChessPos.row][lastChessPos.col] == '-') return;

	// 从棋盘清除上一次下的棋子
	gameBoard[lastChessPos.row][lastChessPos.col] = '-';
	// 更新当前下棋类型
	turnChessType();
}

// 绘制认输按钮文字
void drawTextAdmitDefeat()
{
	nowText = setGameTextType(L"认输", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回

	static int startX = startTextX + midowDx;
	static int startY = startTextY + textNowRow * textHeight + midowDy;
	
	bool pressButton = drawButton(startX, startY, nowText);
	if (pressButton) gameAdmitDefeat();
	
	outtextxy(startX, startY, nowText);
}

void gameAdmitDefeat()
{
	if (nowChessType == 'O') // 如果现在是白棋则黑棋赢了
	{
		gameResult = 2;
	}
	else
	{
		gameResult = 1;
	}
	running = false;

}

// 绘制文本
void drawText()
{
	drawTextTitie(); // 这个必须在第一因为文字起始位置是在这个里面设置的
	drawTextPicture();
	drawTextPrompt();
	drawTextStepBack();
	drawTextAdmitDefeat();
	drawTextWhiteTime();
	drawTextBlackTime();
	drawTextScore();
}


// 渲染
void gameDraw()
{
	BeginBatchDraw(); // 开始画图
	cleardevice(); // 清屏
	drawGridding();
	drawData();
	drawText();
	EndBatchDraw(); // 显示画图
	if (!running) showWinner();
}

// 读取数据
void gameRand()
{
	if (!running) return;

	isClick = false;
	while (peekmessage(&msg))
	{
		if (msg.message == WM_MOUSEMOVE) // 鼠标移动信息
		{
			mouseX = msg.x;
			mouseY = msg.y;
		}
		if (msg.message == WM_LBUTTONDOWN) {

			isClick = true;
		}

	}

}

// 检测棋子是否越界
bool isOverBorder(int& row, int& y)
{
	if (row < 0 || row >= ROWS_WINDOW) return true;
	if (y < 0 || y >= COLS_WINDOW) return true;
	return false;
}

// 检测是否胜利
int chessSum(int row, int col, int dx, int dy)
{
	int sum = 0;
	while (true)
	{
		row += dx;
		col += dy;
		if (isOverBorder(row, col)) return sum;
		if (gameBoard[row][col] == nowChessType) sum++;
		else return sum;
	}
}




// 检测是否结束
bool gameChessOver(int row, int col)
{
	int dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	for (int i = 0; i < 4; i++)
	{
		int sum = 1;
		sum += chessSum(row, col, dir[i][0], dir[i][1]);
		sum += chessSum(row, col, -dir[i][0], -dir[i][1]);
		if (sum == WIN_COUNT)
		{
			if (nowChessType == 'O')
			{
				gameResult = 1;
			}
			else
			{
				gameResult = 2;
			}
			return true;
		}

	}

	if (chessCount == ROWS_WINDOW * COLS_WINDOW)
	{
		gameResult = 0;
		return true;
	}
	return false;
}

bool gameTimeOver() {
	// 倒计时结束的输
	if (blackChessCDTime.h <= 0 && blackChessCDTime.m <= 0 && blackChessCDTime.s <= 0) {
		gameResult = 1;
		return true;
	}
	else if (whiteChessCDTime.h <= 0 && whiteChessCDTime.m <= 0 && whiteChessCDTime.s <= 0) {
		gameResult = 2;
		return true;
	}
	return false;
}

void countdownProcess() {
	// 每过一秒目前倒计时减一
	if (time(0) - nowTimeSecondCD >= 1) {
		nowTimeSecondCD = time(0);
		--(*nowCDTime);
	}

	running = !gameTimeOver();
}


void chessBoardProcess() {
	if (!running) return;

	// 鼠标是否点击
	if (!isClick) return;
	// 判断鼠标是否越界棋盘
	if (mouseX < START_CHECKERBOARD_X || mouseX > START_CHECKERBOARD_X + WIDTH_CHECKERBOARD - col_size ||
		mouseY < START_CHECKERBOARD_Y || mouseY > START_CHECKERBOARD_Y + HEIGHT_CHECKERBOARD - row_size) return;
	// 通过向下取整得出，鼠标点击方块左上角点的行数
	int zeroRow = (mouseY - START_CHECKERBOARD_Y) / row_size;
	int zeroCol = (mouseX - START_CHECKERBOARD_X) / col_size;

	// 求出左上角点的像素坐标
	int zeroSitRow = zeroRow * row_size + START_CHECKERBOARD_Y;
	int zeroSitcol = zeroCol * col_size + START_CHECKERBOARD_X;

	// 初始化下棋位置（默认为左上角）
	int row = zeroRow;
	int col = zeroCol;

	// 根据位置判断实际洛棋点(越界的鼠标信息不会被读取，所以不会越界)
	if (mouseY - zeroSitRow > row_size / 2) row += 1;
	if (mouseX - zeroSitcol > col_size / 2) col += 1;

	if (gameBoard[row][col] != '-') return;
	gameBoard[row][col] = nowChessType;

	// 上一个棋子绘制的行合列
	lastChessPos.row = row;
	lastChessPos.col = col;

	chessCount++; // 棋子个数加一
	running = !gameChessOver(row, col); // 检测是否结束
	turnChessType();
}

// 数据处理
void gameProcess()
{

	// 棋盘数据处理
	chessBoardProcess();

	// 倒计时处理
	countdownProcess();

	

}

// 控制帧率
void gamesleep()
{
	now_time = GetTickCount();
	DWORD delta_ms = now_time - start_time; // 目前运行了多少时间
	DWORD frameMS = 1000 / GAME_FRAME; // 一帧多少毫秒
	if (delta_ms < frameMS)
	{
		Sleep(frameMS - delta_ms); // 休眠到一帧的时间
	}

}





int main()
{
	init();
	while (1)
	{

		while (running)
		{
			gamesleep();
			gameRand();
			gameProcess();
			gameDraw();
		}

		gameRestart();
	}
	return 0;
}



