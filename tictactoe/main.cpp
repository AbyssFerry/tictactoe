#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <iostream>

using namespace std;

#define WIDTH_WINDOW 1400 // 界面宽
#define HEIGHT_WINDOW 900 // 界面高
#define WIDTH_CHECKERBOARD 840 // 棋盘宽  要与列数能整除
#define HEIGHT_CHECKERBOARD 840 // 棋盘高 要与行数能整除
#define START_CHECKERBOARD_X 300 // 棋盘开始绘制x位置
#define START_CHECKERBOARD_Y 25 // 棋盘开始绘制y位置
#define ROWS_WINDOW 30 // 行数
#define COLS_WINDOW 30 // 列数
#define GAME_FRAME 60 // 帧数
#define WIN_COUNT 5 // 连成多少个就赢

int chessCount;
double row_size; // 行大小
double col_size; // 列大小
int mouseX, mouseY;  // 鼠标位置
ExMessage msg; // 记录键盘消息
char chessType; // 第一步棋的类型
bool isClick; // 是否有鼠标点击
bool running = true; // 游戏是否结束
int gameResult; // 0 平局， 1 O赢， 2 X赢
DWORD start_time;
DWORD end_time;

// 储存棋子位置的数组
char gameBoard[ROWS_WINDOW][COLS_WINDOW];

// 初始化
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
	// 胜利提示
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

void drawText()
{
	settextcolor(COLORREF RGB(198, 86, 105));
	TCHAR s[100];
	_stprintf_s(s, sizeof(s) / sizeof(TCHAR), _T("目前的棋子是: %s"), chessType == 'O' ? L"白棋" : L"黑棋");
	outtextxy(0, 0, s);
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



