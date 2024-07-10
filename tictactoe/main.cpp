#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <iostream>

using namespace std;

#define WIDTH_WINDOW 1400 // 界面宽 1400
#define HEIGHT_WINDOW 900 // 界面高 900
#define WIDTH_CHECKERBOARD 840 // 棋盘宽  要与列数能整除
#define HEIGHT_CHECKERBOARD 840 // 棋盘高 要与行数能整除
#define START_CHECKERBOARD_X 500 // 棋盘开始绘制x位置
#define START_CHECKERBOARD_Y 25 // 棋盘开始绘制y位置
#define ROWS_WINDOW 20 // 行数   标准五子棋是15x15
#define COLS_WINDOW 20 // 列数  
#define GAME_FRAME 60 // 帧数
#define WIN_COUNT 5 // 连成多少个就赢
/*-----------------------------------------------------*/

#define TEXT_TITLE 22 // 标题文字占画面高的多少分之一
#define TEXT_PROMPT 30 // 提示文字占画面高的多少分之一
#define TEXT_OTHER 35 // 其他文字占画面高的多少分之一
#define TEXT_ROW 10 // 文本区域行数
/*-----------------------------------------------------*/

int startTextX; // 文字开始绘制x位置（在标题输出函数中初始化）
int startTextY; // 文字开始绘制y位置 （在标题输出函数中初始化）
int textHeight; // 每行文字区域高度 （在标题输出函数中初始化）
int textWeight; // 每行文字区域宽度 （也就是标题的宽度）
int textNowRow; // 代表现在文字输出到第几行 （在标题输出函数中初始化）


int chessCount; // 棋子数量用于平局判定
double row_size; // 行大小
double col_size; // 列大小
int mouseX, mouseY;  // 鼠标位置
ExMessage msg; // 记录键盘消息
char chessType; // 第一步棋的类型
bool isClick; // 是否有鼠标点击
bool running = true; // 游戏是否结束
int gameResult; // 0 平局， 1 O赢， 2 X赢
LOGFONT gameTextType; // 游戏文本输出样式类型
TCHAR* nowText; // 接受文本样式参数返回内容
DWORD start_time; 
DWORD end_time; // 和start_time 实现帧率控制 
/*-----------------------------------------------------*/

// 储存棋子位置的数组
char gameBoard[ROWS_WINDOW][COLS_WINDOW];


int main(); // 主函数
void init(); // 初始化
void gamesleep(); // 控制帧率
void gameRand(); // 读取数据
void gameProcess(); // 数据处理
void gameDraw(); // 渲染
/*-----------------------------------------------------*/

bool gameOver(int row, int col); // 检测是否结束
int chessSum(int row, int col, int dx, int dy); // 检测是否胜利
bool isOverBorder(int& row, int& y); // 检测棋子是否越界
/*-----------------------------------------------------*/

int centeringLRText(TCHAR* s); // 返回文字居中所需的x偏移量
int centeringHWText(TCHAR* s); // 返回文字居中所需的y偏移量

void drawData(); // 绘制数据
void drawText(); // 绘制文本
TCHAR* setGameTextType(const TCHAR content[100] = _T("默认输出"), COLORREF colorRGB = RGB(0, 0, 0), double height = 10); // 设置文本样式参数
void drawTextTitie(); // 绘制标题文字
void drawTextRestart(); // 绘制重新开始按钮文字
void drawTextStepFront(); // 绘制进一步按钮文字
void drawTextStepBack(); // 绘制退一步按钮文字
void drawTextPrompt(); // 绘制提示文字
void showWinner(); // 绘制输赢画面
void drawCircle(int x, int y, int r); // 绘制棋子画圆
void drawGridding(); // 绘制网格
/*-----------------------------------------------------*/




void init()
{
	initgraph(WIDTH_WINDOW, HEIGHT_WINDOW);
	// 计算行列的宽
	row_size = HEIGHT_CHECKERBOARD / ROWS_WINDOW;
	col_size = WIDTH_CHECKERBOARD / COLS_WINDOW;

	for(int i = 0; i < ROWS_WINDOW; i++)
	{
		for(int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}
	// 定义一个 RGB 颜色值，使用宏 RGB(r, g, b)
	COLORREF bgColor = RGB(236, 208, 133); // 棋盘肉棕色

	// 设置背景填充色
	setbkcolor(bgColor);

	mouseX = -1;
	mouseY = -1;

	chessType = 'X';
	isClick = false;
	start_time = GetTickCount();  // 获取开机到现在的时间
	chessCount = 0;
	gameResult = -1;


	
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
}

// 画圆
void drawCircle(int x, int y, int r)
{
	solidcircle(x, y, r);
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
			int r = min(row_size / 2, col_size / 2);

			if(gameBoard[i][j] == 'O')
			{
				setfillcolor(COLORREF(RGB(255, 255, 255))); // 设为白色
				drawCircle(colSit, rowSit, r);
			}else if(gameBoard[i][j] == 'X')
			{
				setfillcolor(COLORREF(RGB(0, 0, 0))); // 设为黑色
				drawCircle(colSit, rowSit, r);
			}
		}

	}
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
	}
	else if(gameResult == 2)
	{
		MessageBox(GetHWnd(), _T("黑棋玩家胜利○( ＾皿＾)っHiahiahia…"), _T("胜负情况"), MB_OK);
	}

}

// 返回文字居中所需的x偏移量
int centeringLRText(TCHAR *s)
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
	nowText = setGameTextType(L"来吧开始博弈！！！", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_TITLE);

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
	static TCHAR tempText [100]; // 用于拼接
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("目前的棋子是: %s"), chessType == 'O' ? L"白棋" : L"黑棋");
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_PROMPT);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText); // 居中x偏量返回
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);

}



// 绘制退一步按钮文字
void drawTextStepBack()
{
	nowText = setGameTextType(L"反方向的钟", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);
	

	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

// 绘制进一步按钮文字
void drawTextStepFront()
{
	nowText = setGameTextType(L"人要往前看", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

// 绘制重新开始按钮文字
void drawTextRestart()
{
	nowText = setGameTextType(L"不服重来", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // 当前行加一
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // 居中y偏量返回
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

// 绘制文本
void drawText()
{
	drawTextTitie(); // 这个必须在第一因为文字起始位置是在这个里面设置的
	drawTextPrompt();
	drawTextStepBack();
	drawTextStepFront();
	drawTextRestart();
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
	while(peekmessage(&msg))
	{
		if(msg.message == WM_LBUTTONDOWN)
		{
			// 判断鼠标是否越界
			if (msg.x < START_CHECKERBOARD_X || msg.x > START_CHECKERBOARD_X + WIDTH_CHECKERBOARD - col_size) return;
			if (msg.y < START_CHECKERBOARD_Y || msg.y > START_CHECKERBOARD_Y + HEIGHT_CHECKERBOARD - row_size) return;
			mouseX = msg.x;
			mouseY = msg.y;
			isClick = true;
		}
	}
}

// 检测棋子是否越界
bool isOverBorder(int & row, int &y)
{
	if (row < 0 || row >= ROWS_WINDOW) return true;
	if (y < 0 || y >= COLS_WINDOW) return true;
	return false;
}

// 检测是否胜利
int chessSum(int row, int col, int dx, int dy)
{
	int sum = 0;
	while(true)
	{
		row += dx;
		col += dy;
		if (isOverBorder(row, col)) return sum;
		if (gameBoard[row][col] == chessType) sum++;
		else return sum;
	}
}




// 检测是否结束
bool gameOver(int row, int col)
{
	int dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1}};
	for(int i = 0; i < 4; i++)
	{
		int sum = 1;
		sum += chessSum(row, col, dir[i][0], dir[i][1]);
		sum += chessSum(row, col, -dir[i][0], -dir[i][1]);
		if(sum == WIN_COUNT)
		{
			if(chessType == 'O')
			{
				gameResult = 1;
			}else
			{
				gameResult = 2;
			}
			return false;			
		}

	}

	if (chessCount == ROWS_WINDOW * COLS_WINDOW)
	{
		gameResult = 0;
		return false;
	}
	return true;
}


// 数据处理
void gameProcess()
{
	if (!isClick) return;

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
	gameBoard[row][col] = chessType;
	chessCount++; // 棋子个数加一
	running = gameOver(row, col); // 检测是否结束
	if (chessType == 'X') chessType = 'O';
	else chessType = 'X';
	isClick = false;

}

// 控制帧率
void gamesleep()
{
	end_time = GetTickCount();
	DWORD delta_ms = end_time - start_time; // 目前运行了多少时间
	DWORD frameMS = 1000 / GAME_FRAME; // 一帧多少毫秒
	if(delta_ms < frameMS)
	{
		Sleep(frameMS - delta_ms); // 休眠到一帧的时间
	}

}





int main()
{
	init();

	while (running)
	{
		gamesleep();
		gameRand();
		gameProcess();
		gameDraw();
		
	}
	
	return 0;
}



