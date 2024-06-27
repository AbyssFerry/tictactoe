#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <iostream>

using namespace std;

#define WIDTH_WINDOW 1400 // �����
#define HEIGHT_WINDOW 900 // �����
#define WIDTH_CHECKERBOARD 840 // ���̿�  Ҫ������������
#define HEIGHT_CHECKERBOARD 840 // ���̸� Ҫ������������
#define START_CHECKERBOARD_X 500 // ���̿�ʼ����xλ��
#define START_CHECKERBOARD_Y 25 // ���̿�ʼ����yλ��
#define ROWS_WINDOW 20 // ����   ��׼��������15x15
#define COLS_WINDOW 20 // ����  
#define GAME_FRAME 60 // ֡��
#define WIN_COUNT 5 // ���ɶ��ٸ���Ӯ
/*-----------------------------------------------------*/

#define TEXT_TITLE 22 // ��������ռ����ߵĶ��ٷ�֮һ
#define TEXT_PROMPT 30 // ��ʾ����ռ����ߵĶ��ٷ�֮һ
#define TEXT_OTHER 35 // ��������ռ����ߵĶ��ٷ�֮һ
#define TEXT_ROW 10 // �ı���������
/*-----------------------------------------------------*/

int startTextX; // ���ֿ�ʼ����xλ�ã��ڱ�����������г�ʼ����
int startTextY; // ���ֿ�ʼ����yλ�� ���ڱ�����������г�ʼ����
int textHeight; // ÿ����������߶� ���ڱ�����������г�ʼ����
int textWeight; // ÿ������������ ��Ҳ���Ǳ���Ŀ�ȣ�
int textNowRow; // ������������������ڼ��� ���ڱ�����������г�ʼ����


int chessCount; // ������������ƽ���ж�
double row_size; // �д�С
double col_size; // �д�С
int mouseX, mouseY;  // ���λ��
ExMessage msg; // ��¼������Ϣ
char chessType; // ��һ���������
bool isClick; // �Ƿ��������
bool running = true; // ��Ϸ�Ƿ����
int gameResult; // 0 ƽ�֣� 1 OӮ�� 2 XӮ
LOGFONT gameTextType; // ��Ϸ�ı������ʽ����
TCHAR* nowText; // �����ı���ʽ������������
DWORD start_time; 
DWORD end_time; // ��start_time ʵ��֡�ʿ��� 
/*-----------------------------------------------------*/

// ��������λ�õ�����
char gameBoard[ROWS_WINDOW][COLS_WINDOW];


int main(); // ������
void init(); // ��ʼ��
void gamesleep(); // ����֡��
void gameRand(); // ��ȡ����
void gameProcess(); // ���ݴ���
void gameDraw(); // ��Ⱦ
/*-----------------------------------------------------*/

bool gameOver(int row, int col); // ����Ƿ����
int chessSum(int row, int col, int dx, int dy); // ����Ƿ�ʤ��
bool isOverBorder(int& row, int& y); // ��������Ƿ�Խ��
/*-----------------------------------------------------*/

int centeringText(TCHAR* s); // �������־��������xƫ����

void drawData(); // ��������
void drawText(); // �����ı�
TCHAR* setGameTextType(const TCHAR content[100] = _T("Ĭ�����"), COLORREF colorRGB = RGB(0, 0, 0), double height = 10); // �����ı���ʽ����
void drawTextTitie(); // ���Ʊ�������
void drawTextRestart(); // �������¿�ʼ��ť����
void drawTextStepFront(); // ���ƽ�һ����ť����
void drawTextStepBack(); // ������һ����ť����
void drawTextPrompt(); // ������ʾ����
void showWinner(); // ������Ӯ����
void drawCircle(int x, int y, int r); // �������ӻ�Բ
void drawGridding(); // ��������
/*-----------------------------------------------------*/




void init()
{
	initgraph(WIDTH_WINDOW, HEIGHT_WINDOW);
	// �������еĿ�
	row_size = HEIGHT_CHECKERBOARD / ROWS_WINDOW;
	col_size = WIDTH_CHECKERBOARD / COLS_WINDOW;

	for(int i = 0; i < ROWS_WINDOW; i++)
	{
		for(int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}
	// ����һ�� RGB ��ɫֵ��ʹ�ú� RGB(r, g, b)
	COLORREF bgColor = RGB(236, 208, 133); // ��������ɫ

	// ���ñ������ɫ
	setbkcolor(bgColor);

	mouseX = -1;
	mouseY = -1;

	chessType = 'X';
	isClick = false;
	start_time = GetTickCount();  // ��ȡ���������ڵ�ʱ��
	chessCount = 0;
	gameResult = -1;


	
}

// ��������
void drawGridding()
{
	COLORREF lineColor = RGB(0, 0, 0); // ��ɫ
	setlinecolor(lineColor);

	// ���û��ߵ�ƫ����
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

// ��Բ
void drawCircle(int x, int y, int r)
{
	solidcircle(x, y, r);
}


// ��������
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
				setfillcolor(COLORREF(RGB(255, 255, 255))); // ��Ϊ��ɫ
				drawCircle(colSit, rowSit, r);
			}else if(gameBoard[i][j] == 'X')
			{
				setfillcolor(COLORREF(RGB(0, 0, 0))); // ��Ϊ��ɫ
				drawCircle(colSit, rowSit, r);
			}
		}

	}
}

void showWinner()
{
	// ʤ����ʾ
	if (gameResult == 0)
	{
		MessageBox(GetHWnd(), _T("���ƽ��"), _T("ʤ�����"), MB_OK);
	}
	else if (gameResult == 1)
	{
		MessageBox(GetHWnd(), _T("�������ʤ��(��y����)�q Ohohoho....."), _T("ʤ�����"), MB_OK);
	}
	else if(gameResult == 2)
	{
		MessageBox(GetHWnd(), _T("�������ʤ����( �����)��Hiahiahia��"), _T("ʤ�����"), MB_OK);
	}

}

// �������־��������xƫ����
int centeringText(TCHAR *s)
{
	return (textWeight - textwidth(s)) / 2;
}

TCHAR* setGameTextType(const TCHAR content[100], COLORREF colorRGB, double height)
{
	settextcolor(colorRGB);  // ��ɫ
	gettextstyle(&gameTextType);  // ��ȡ��ǰ������ʽ

	gameTextType.lfHeight = height;  // ���ݱ���������ʾ���ֵĸ�

	settextstyle(&gameTextType);  // �����ĺ����ʽ������

	static TCHAR s[100]; // �����������飬��ʼ��һ�ξ��У��Ժ�ֻҪ��ֵ
	_stprintf_s(s, sizeof(s) / sizeof(TCHAR), _T("%s"), content);
	return s;
}

// ���Ʊ�������
void drawTextTitie()
{
	nowText = setGameTextType(L"���ɿ�ʼ���ģ�����", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_TITLE);

	// ͨ�������ʼ��λ���Լ�ÿ�����ָ߶�
	static int firstFlag = true; // ֻ��ʼ��һ��
	if (firstFlag)
	{
		firstFlag = false;  // ֻ��ִ��һ��������ʼλ��
		textWeight = textwidth(nowText);
		startTextX = (START_CHECKERBOARD_X - textWeight) / 2; // ʹ��������λ������������ȡ����м�
		startTextY = startTextX; // Ŀǰ�Ⱦ���y������ʼ��������Ϊ��x������ʼ����һ��
		textHeight = (HEIGHT_WINDOW - 2 * startTextY) / TEXT_ROW; // ����ÿ�����ִ�С
	}

	// ÿ����������ⶼ��0�п�ʼ
	textNowRow = 0;

	outtextxy(startTextX, startTextY + textNowRow * textHeight, nowText);

}

// ������ʾ����
void drawTextPrompt()
{
	static TCHAR tempText [100]; // ����ƴ��
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("Ŀǰ��������: %s"), chessType == 'O' ? L"����" : L"����");
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_PROMPT);


	int midowDx = centeringText(nowText); // ����ƫ������
	textNowRow++; // ��ǰ�м�һ
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight, nowText);

}



// ������һ����ť����
void drawTextStepBack()
{
	nowText = setGameTextType(L"���������", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);
	

	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringText(nowText);
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight, nowText);
}

// ���ƽ�һ����ť����
void drawTextStepFront()
{
	nowText = setGameTextType(L"��Ҫ��ǰ��", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringText(nowText);
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight, nowText);
}

// �������¿�ʼ��ť����
void drawTextRestart()
{
	nowText = setGameTextType(L"��������", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringText(nowText);
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight, nowText);
}

// �����ı�
void drawText()
{
	drawTextTitie(); // ��������ڵ�һ��Ϊ������ʼλ����������������õ�
	drawTextPrompt();
	drawTextStepBack();
	drawTextStepFront();
	drawTextRestart();
}


// ��Ⱦ
void gameDraw()
{
	BeginBatchDraw(); // ��ʼ��ͼ
	cleardevice(); // ����
	drawGridding();
	drawData();
	drawText();
	EndBatchDraw(); // ��ʾ��ͼ
	if (!running) showWinner();
}

// ��ȡ����
void gameRand()
{
	while(peekmessage(&msg))
	{
		if(msg.message == WM_LBUTTONDOWN)
		{
			// �ж�����Ƿ�Խ��
			if (msg.x < START_CHECKERBOARD_X || msg.x > START_CHECKERBOARD_X + WIDTH_CHECKERBOARD - col_size) return;
			if (msg.y < START_CHECKERBOARD_Y || msg.y > START_CHECKERBOARD_Y + HEIGHT_CHECKERBOARD - row_size) return;
			mouseX = msg.x;
			mouseY = msg.y;
			isClick = true;
		}
	}
}

// ��������Ƿ�Խ��
bool isOverBorder(int & row, int &y)
{
	if (row < 0 || row >= ROWS_WINDOW) return true;
	if (y < 0 || y >= COLS_WINDOW) return true;
	return false;
}

// ����Ƿ�ʤ��
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




// ����Ƿ����
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


// ���ݴ���
void gameProcess()
{
	if (!isClick) return;

	// ͨ������ȡ���ó���������������Ͻǵ������
	int zeroRow = (mouseY - START_CHECKERBOARD_Y) / row_size;
	int zeroCol = (mouseX - START_CHECKERBOARD_X) / col_size;

	// ������Ͻǵ����������
	int zeroSitRow = zeroRow * row_size + START_CHECKERBOARD_Y;
	int zeroSitcol = zeroCol * col_size + START_CHECKERBOARD_X;

	// ��ʼ������λ�ã�Ĭ��Ϊ���Ͻǣ�
	int row = zeroRow;
	int col = zeroCol;

	// ����λ���ж�ʵ�������(Խ��������Ϣ���ᱻ��ȡ�����Բ���Խ��)
	if (mouseY - zeroSitRow > row_size / 2) row += 1;
	if (mouseX - zeroSitcol > col_size / 2) col += 1;

	if (gameBoard[row][col] != '-') return;
	gameBoard[row][col] = chessType;
	chessCount++; // ���Ӹ�����һ
	running = gameOver(row, col); // ����Ƿ����
	if (chessType == 'X') chessType = 'O';
	else chessType = 'X';
	isClick = false;

}

// ����֡��
void gamesleep()
{
	end_time = GetTickCount();
	DWORD delta_ms = end_time - start_time; // Ŀǰ�����˶���ʱ��
	DWORD frameMS = 1000 / GAME_FRAME; // һ֡���ٺ���
	if(delta_ms < frameMS)
	{
		Sleep(frameMS - delta_ms); // ���ߵ�һ֡��ʱ��
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



