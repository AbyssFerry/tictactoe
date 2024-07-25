#include <graphics.h>
#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <iostream>
#include <ctime>

// ʵ����void turnChessType(); // ת����ǰ��������
// �����˽ṹ��chessPosRowAndCol {int row;int col;}
// ʵ���˻��庯��chessUndo() �Լ��������

using namespace std;

#define WIDTH_WINDOW 1400 // ����� 1400
#define HEIGHT_WINDOW 900 // ����� 900
#define WIDTH_CHECKERBOARD 840 // ���̿�  Ҫ������������
#define HEIGHT_CHECKERBOARD 840 // ���̸� Ҫ������������
#define START_CHECKERBOARD_X 500 // ���̿�ʼ����xλ��
#define START_CHECKERBOARD_Y 25 // ���̿�ʼ����yλ��
#define ROWS_WINDOW 21 // ����   ��׼��������15x15
#define COLS_WINDOW 21 // ����  
#define GAME_FRAME 60 // ֡��
#define WIN_COUNT 5 // ���ɶ��ٸ���Ӯ
/*-----------------------------------------------------*/

#define TEXT_TITLE 22 // ��������ռ����ߵĶ��ٷ�֮һ
#define TEXT_PROMPT 30 // ��ʾ����ռ����ߵĶ��ٷ�֮һ
#define TEXT_OTHER 35 // ��������ռ����ߵĶ��ٷ�֮һ
#define TEXT_ROW 10 // �ı���������
#define BUTTON_HIGH 30 // ��ť��ռ����߶���
#define BUTTON_WIDE 3// ��ť��ռ��������
/*-----------------------------------------------------*/
#define BACKGROUND_COLLOR RGB(236, 208, 133)
#define BUTTON_COLLOR_ORINGE RGB(128, 128, 128) 
#define BUTTON_COLLOR_ACTIVATE RGB(160, 160, 160)

/*-----------------------------------------------------*/
int startTextX; // ���ֿ�ʼ����xλ�ã��ڱ�����������г�ʼ����
int startTextY; // ���ֿ�ʼ����yλ�� ���ڱ�����������г�ʼ����
int textHeight; // ÿ����������߶� ���ڱ�����������г�ʼ����
int textWeight; // ÿ������������ ��Ҳ���Ǳ���Ŀ�ȣ�
int textNowRow; // ������������������ڼ��� ���ڱ�����������г�ʼ����

// ���ڴ�����������
typedef struct chessPosRowAndCol {
	int row;
	int col;
};

// ���ڵ���ʱ
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

int chessCount; // ������������ƽ���ж�
double row_size; // �д�С
double col_size; // �д�С
int mouseX, mouseY;  // ���λ��
ExMessage msg; // ��¼������Ϣ
char nowChessType; // ��ǰ������� X�� O��
chessPosRowAndCol lastChessPos; // ��¼��һ�����ӵ�λ�ã�����ʵ�ֻ���
bool isClick; // �Ƿ��������
bool running; // ��Ϸ�Ƿ����
int gameResult; // 0 ƽ�֣� 1 OӮ�� 2 XӮ
int blackScore, whiteScore; // ����Ͱ���÷�
Time blackChessCDTime; // ���嵹��ʱ
Time whiteChessCDTime; // ���嵹��ʱ
Time* nowCDTime; // ���ڵĵ���ʱ
time_t nowTimeSecondCD; // ���ڵ���ʱ��ʱ��
LOGFONT gameTextType; // ��Ϸ�ı������ʽ����
TCHAR* nowText; // �����ı���ʽ������������
DWORD start_time;
DWORD now_time; // ��start_time ʵ��֡�ʿ��� 
/*-----------------------------------------------------*/

// ��������λ�õ�����
char gameBoard[ROWS_WINDOW][COLS_WINDOW];


int main(); // ������
void init(); // ��ʼ��
void gamesleep(); // ����֡��
void gameRand(); // ��ȡ����
void gameProcess(); // ���ݴ���
void gameDraw(); // ��Ⱦ
void gameRestart(); // ���¿�ʼ�µ�һ��
void turnChessType(); // ת����ǰ��������
/*-----------------------------------------------------*/

bool gameChessOver(int row, int col); // ����Ƿ��������ɻ�ʤ����������������ƽ��
bool gameTimeOver(); // ��⵹��ʱ�Ƿ����
int chessSum(int row, int col, int dx, int dy); // ����Ƿ�ʤ��
bool isOverBorder(int& row, int& y); // ��������Ƿ�Խ��
/*-----------------------------------------------------*/
void chessBoardProcess(); // �������ݴ���
void countdownProcess(); // ����ʱ����


/*-----------------------------------------------------*/
int centeringLRText(TCHAR* s); // �������־��������xƫ����
int centeringHWText(TCHAR* s); // �������־��������yƫ����


void drawData(); // ��������
void drawText(); // �����ı�
void drawGridding(); // ��������
void drawGriddingPoint(); // ���������ϵİ��
TCHAR* setGameTextType(const TCHAR content[100] = _T("Ĭ�����"), COLORREF colorRGB = RGB(0, 0, 0), double height = 10); // �����ı���ʽ����
void drawTextTitie(); // ���Ʊ�������
void drawTextAdmitDefeat(); // �������䰴ť����
void gameAdmitDefeat(); // ����
void drawTextStepBack(); // ���ƻ��尴ť����
void chessUndo(); // ����
void drawTextPicture(); // ����ͼƬ
void drawTextPicture_doubleManPlayChess(int startX, int startY); // �������������廭��
void drawTextBlackTime(); // ���ƺ���ʣ��ʱ��
void drawTextWhiteTime(); // ���ư���ʣ��ʱ��
void drawTextScore(); // ���Ʊȷ�
void drawTextPrompt(); // ������ʾ����
void showWinner(); // ������Ӯ����
void drawCircle(int x, int y, int r); // �������ӻ�Բ
bool drawButton(int x, int y, TCHAR* text); // ���ư�ť�� ���ذ�ť�Ƿ񱻴���
/*-----------------------------------------------------*/




void init()
{
	initgraph(WIDTH_WINDOW, HEIGHT_WINDOW);
	// �������еĿ�
	row_size = HEIGHT_CHECKERBOARD / ROWS_WINDOW;
	col_size = WIDTH_CHECKERBOARD / COLS_WINDOW;

	for (int i = 0; i < ROWS_WINDOW; i++)
	{
		for (int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}

	// ���ñ������ɫ
	setbkcolor(COLORREF(BACKGROUND_COLLOR));

	// �տ��ǵ÷ֶ�Ϊ0
	blackScore = 0;
	whiteScore = 0;

	// ��ʼ������ʱΪ5����, ���Һڷ���ʼ��ʱ
	blackChessCDTime = Time(0, 0, 10);
	whiteChessCDTime = Time(0, 0, 10);
	nowCDTime = &blackChessCDTime;

	// ��Ϸ��ʼ����
	running = true;

	// һЩ��ʼ��
	mouseX = -1;
	mouseY = -1;
	nowChessType = 'X';
	isClick = false;
	start_time = GetTickCount();  // ��ȡ���������ڵ�ʱ��
	nowTimeSecondCD = time(0); // ����1970��1��1����ҹ����������ʱ�䣬����Ϊ��λ��
	chessCount = 0;
	gameResult = -1;



}

void gameRestart() {
	running = true; // ��Ϸ������Ϊtrue

	// �������
	for (int i = 0; i < ROWS_WINDOW; i++)
	{
		for (int j = 0; j < COLS_WINDOW; j++)
		{
			gameBoard[i][j] = '-';
		}
	}

	// ��ʼ��Ϊ��
	nowChessType = 'X';

	// ����ʱ����
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

	drawGriddingPoint(); // ���������ϵİߵ�
}

void drawGriddingPoint() {
	setfillcolor(COLORREF(RGB(0, 0, 0))); // ����Ϊ��ɫ
	int R = min(col_size, row_size) / 9;
	int Dxy = 5; // �Ա�4����������Ķ�������

	int middleRow = ROWS_WINDOW / 2;
	int middleCol = COLS_WINDOW / 2;
	int middleX = START_CHECKERBOARD_X + col_size * middleCol;
	int middleY = START_CHECKERBOARD_Y + row_size * middleRow;
	fillcircle(middleX, middleY, R); // �������ĵ�

	int left_x = START_CHECKERBOARD_X + (middleCol - Dxy) * col_size;
	int right_x = START_CHECKERBOARD_X + (middleCol + Dxy) * col_size;
	int top_y = START_CHECKERBOARD_Y + (middleRow - Dxy) * row_size;
	int down_y = START_CHECKERBOARD_Y + (middleRow + Dxy) * row_size;
	fillcircle(left_x, top_y, R);
	fillcircle(left_x, down_y, R);
	fillcircle(right_x, top_y, R);
	fillcircle(right_x, down_y, R);
}

// ��Բ
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
		setfillcolor(BUTTON_COLLOR_ACTIVATE); // ����ھͻ��Ƽ�����ɫ
		solidrectangle(x1, y1, x2, y2);
		if (isClick) return true; // ���������˾ʹ���ť����
	}
	else {
		setfillcolor(BUTTON_COLLOR_ORINGE);
		solidrectangle(x1, y1, x2, y2);
	}

	return false;
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
			int r = min(row_size / 2 - 3, col_size / 2 - 3);

			if (gameBoard[i][j] == 'O')
			{
				setfillcolor(COLORREF(RGB(255, 255, 255))); // ��Ϊ��ɫ
				setlinecolor(COLORREF(RGB(0, 0, 0)));
				drawCircle(colSit, rowSit, r);
			}
			else if (gameBoard[i][j] == 'X')
			{
				setfillcolor(COLORREF(RGB(0, 0, 0))); // ��Ϊ��ɫ
				setlinecolor(COLORREF(RGB(255, 255, 255)));
				drawCircle(colSit, rowSit, r);
			}
		}

	}
	setlinecolor(COLORREF(RGB(0, 0, 0))); // �ѻ�����ɫ��ΪĬ�Ϻ�
}

void showWinner()
{
	// ʤ����ʾ  GetHWnd���ڵ���ϵͳ����
	if (gameResult == 0)
	{
		MessageBox(GetHWnd(), _T("���ƽ��"), _T("ʤ�����"), MB_OK);
	}
	else if (gameResult == 1)
	{
		MessageBox(GetHWnd(), _T("�������ʤ��(��y����)�q Ohohoho....."), _T("ʤ�����"), MB_OK);
		whiteScore++; // �����һ��
	}
	else if (gameResult == 2)
	{
		MessageBox(GetHWnd(), _T("�������ʤ����( �����)��Hiahiahia��"), _T("ʤ�����"), MB_OK);
		blackScore++; // �����һ��
	}

}

// �������־��������xƫ����
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
	midowDx = centeringLRText(nowText); // ����xƫ������
	midowDy = centeringHWText(nowText); // ����yƫ������
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

TCHAR* setGameTextType(const TCHAR content[100], COLORREF colorRGB, double height)
{
	settextcolor(colorRGB);  // ��ɫ

	setbkmode(TRANSPARENT); // �������ֱ���ģʽΪ͸��

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
	nowText = setGameTextType(L"===���ɿ�ʼ����===", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_TITLE);

	// ͨ�������ʼ��λ���Լ�ÿ�����ָ߶�
	static int firstFlag = true; // ֻ��ʼ��һ��
	if (firstFlag)
	{
		firstFlag = false;  // ֻ��ִ��һ��������ʼλ��
		textWeight = textwidth(nowText);
		startTextX = (START_CHECKERBOARD_X - textWeight) / 2; // ʹ��������λ������������ȡ����м�
		startTextY = startTextX; // Ŀǰ�Ⱦ���y������ʼ��������Ϊ��x������ʼ����һ��
		textHeight = (HEIGHT_WINDOW - 2 * startTextY) / TEXT_ROW; // ������������ÿ�д�С
	}

	// ÿ����������ⶼ��0�п�ʼ
	textNowRow = 0;

	outtextxy(startTextX, startTextY + textNowRow * textHeight, nowText);

}

// ������ʾ����
void drawTextPrompt()
{
	static TCHAR tempText[100]; // ����ƴ��
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("Ŀǰ��������: %s"), nowChessType == 'O' ? L"����" : L"����");
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_PROMPT);


	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText); // ����xƫ������
	int midowDy = centeringHWText(nowText); // ����yƫ������
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);

}

void drawTextPicture_doubleManPlayChess(int startX, int startY) {
	// �����ͷ
	int lheadX = 20, lheadY = 20, lheadR = 40;
	circle(startX + lheadX, startY + lheadY, lheadR);
	// ���������
	int lbodyLen = 60;
	line(startX + lheadX, startY + lheadY + lheadR, startX + lheadX, startY + lheadY + lheadR + lbodyLen);
	// �������1
	int lhand1X = 120, lhand1Y = 10, lhandDy = lbodyLen / 2 - 10;
	line(startX + lheadX, startY + lheadY + lheadR + lhandDy, startX + lheadX + lhand1X, startY + lheadY + lheadR + lhandDy + lhand1Y);
	// �������2
	int lhand21X = 50, lhand21Y = 30;
	int lhand22X = 41, lhand22Y = 55;
	line(startX + lheadX, startY + lheadY + lheadR + lhandDy, startX + lheadX + lhand21X, startY + lheadY + lheadR + lhandDy + lhand21Y);
	line(startX + lheadX + lhand21X, startY + lheadY + lheadR + lhandDy + lhand21Y, startX + lhand22X, startY + lhand22Y);
	// �����11
	int lleg11X = 60, lleg11Y = 5;
	line(startX + lheadX, startY + lheadY + lheadR + lbodyLen, startX + lheadX + lleg11X, startY + lheadY + lheadR + lbodyLen + lleg11Y);
	// �����12
	int lleg12X = -5, lleg12Y = 70;
	line(startX + lheadX + lleg11X, startY + lheadY + lheadR + lbodyLen + lleg11Y, startX + lheadX + lleg11X + lleg12X, startY + lheadY + lheadR + lbodyLen + lleg11Y + lleg12Y);
	// �����21
	int lleg21X = 40, lleg21Y = 15;
	line(startX + lheadX, startY + lheadY + lheadR + lbodyLen, startX + lheadX + lleg21X, startY + lheadY + lheadR + lbodyLen + lleg21Y);
	// �����22
	int lleg22X = -5, lleg22Y = 60;
	line(startX + lheadX + lleg21X, startY + lheadY + lheadR + lbodyLen + lleg21Y, startX + lheadX + lleg21X + lleg22X, startY + lheadY + lheadR + lbodyLen + lleg21Y + lleg22Y);

	// ��������ŵĺ���
	int lTakeChessX = startX + lheadX + lhand1X, lTakeChessY = startY + lheadY + lheadR + lhandDy + lhand1Y;
	int lTakeChessR = 5;
	setfillcolor(COLORREF(RGB(0, 0, 0)));
	fillcircle(lTakeChessX, lTakeChessY, lTakeChessR);

	// �������
	int lseatAx = startX + lheadX - 20, lseatAy = startY + lheadY + lheadR + lbodyLen + 5;
	int lseatWide = 40, lseatHigh = 75, lseatDy = 10, lseatDx = lseatDy / 2 * 5;
	/*  ͼ�����ͼ
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


	// ����
	int tableAX = startX + lheadX + 63, tableAY = startY + lheadY + lheadR + lbodyLen + lseatDy / 2;
	int tableHiht = 80, tableWide = 140, tableDy = 40, tableDx = tableDy / 2 * 5;
	/*  ͼ�����ͼ
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

	// �������ϵ���
	int tableLineDx = tableWide / 10, tableLineDy = tableDy / 10;
	for (int i = 1; i < 10; i++) {
		line(tableAX + i * tableLineDx, tableAY, tableAX + tableDx + i * tableLineDx, tableAY - tableDy);
		line(tableAX + i * tableLineDy / 2 * 5, tableAY - i * tableLineDy, tableAX + i * tableLineDy / 2 * 5 + tableWide, tableAY - i * tableLineDy);
	}

	// �ұ�����
	int rseatAX = tableAX + tableWide + 63, rseatAY = tableAY;
	int rseatHigh = 75, rseatWide = 40, rseatDy = 10, rseatDx = rseatDy / 2 * 5;
	/*  ͼ�����ͼ
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

	// �ұ�����
	int rbodyLen = 60;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide, rseatAY - rseatDy / 2 - rbodyLen);

	// �ұ���ͷ
	int rheadR = 40, rheadDx = rseatAX + rseatWide, rheadDy = rseatAY - rseatDy / 2 - rbodyLen - rheadR;
	circle(rheadDx, rheadDy, rheadR);

	// �ұ߽�11
	int rleg11X = 20, rleg11Y = 18;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide - rleg11X, rseatAY - rseatDy / 2 - rleg11Y);

	// �ұ߽�12
	int rleg12X = 30, rleg12Y = 85;
	line(rseatAX + rseatWide - rleg11X, rseatAY - rseatDy / 2 - rleg11Y, rseatAX + rseatWide - rleg11X - rleg12X, rseatAY - rseatDy / 2 - rleg11Y + rleg12Y);

	// �ұ߽�21
	int rleg21X = -15, rleg21Y = 1;
	line(rseatAX + rseatWide, rseatAY - rseatDy / 2, rseatAX + rseatWide - rleg12X + rleg21X, rseatAY - rseatDy / 2 + rleg21Y);

	// �ұ߽�22
	int rleg22X = 5, rleg22Y = 25;
	line(rseatAX + rseatWide - rleg12X + rleg21X, rseatAY - rseatDy / 2 + rleg21Y, rseatAX + rseatWide - rleg12X + rleg21X + rleg22X, rseatAY - rseatDy / 2 + rleg21Y + rleg22Y);

	// �ұ���
	int rhandStartX = rseatAX + rseatWide, rhandStartY = rseatAY - rseatDy / 2 - rbodyLen / 2 - 10;
	// �ұ���1
	int rhand1X = rseatAX + rseatWide - rleg11X - rleg12X, rhand1Y = rseatAY - rseatDy / 2 - rleg11Y + rleg12Y;
	line(rhandStartX, rhandStartY, rhand1X, rhand1Y);

	// �ұ���2
	int rhand2X = rseatAX + rseatDx + rseatWide - 15, rhand2Y = rseatAY - rseatDy + 3;
	line(rhandStartX, rhandStartY, rhand2X, rhand2Y);
}

void drawTextPicture() {
	nowText = setGameTextType(L"", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // ��ǰ�м�һ
	int midowDx = 0;
	int midowDy = 0; // ͼƬ����Ҫxy����

	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
	drawTextPicture_doubleManPlayChess(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy);

	textNowRow += 2; // ΪͼƬ��������ռ�
}

void drawTextScore() {
	static TCHAR tempText[100]; // ����ƴ��
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("��:%d  Vs  ��:%d"), blackScore, whiteScore);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // ����yƫ������
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

void drawTextWhiteTime() {
	static TCHAR tempText[100]; // ����ƴ��
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("�� %02d:%02d"), whiteChessCDTime.m, whiteChessCDTime.s);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // ����yƫ������
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

void drawTextBlackTime()
{
	static TCHAR tempText[100]; // ����ƴ��
	_stprintf_s(tempText, sizeof(tempText) / sizeof(TCHAR), _T("�� %02d:%02d"), blackChessCDTime.m, blackChessCDTime.s);
	nowText = setGameTextType(tempText, RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // ����yƫ������
	outtextxy(startTextX + midowDx, startTextY + textNowRow * textHeight + midowDy, nowText);
}

// ���ƻ��尴ť����
void drawTextStepBack()
{
	nowText = setGameTextType(L"����", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);

	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // ����yƫ������

	static int startX = startTextX + midowDx;
	static int startY = startTextY + textNowRow * textHeight + midowDy;

	bool pressButton = drawButton(startX, startY, nowText);
	if (pressButton) chessUndo();
	outtextxy(startX, startY, nowText);
}

void chessUndo()
{
	if (gameBoard[lastChessPos.row][lastChessPos.col] == '-') return;

	// �����������һ���µ�����
	gameBoard[lastChessPos.row][lastChessPos.col] = '-';
	// ���µ�ǰ��������
	turnChessType();
}

// �������䰴ť����
void drawTextAdmitDefeat()
{
	nowText = setGameTextType(L"����", RGB(0, 0, 0), HEIGHT_WINDOW / TEXT_OTHER);


	textNowRow++; // ��ǰ�м�һ
	int midowDx = centeringLRText(nowText);
	int midowDy = centeringHWText(nowText); // ����yƫ������

	static int startX = startTextX + midowDx;
	static int startY = startTextY + textNowRow * textHeight + midowDy;
	
	bool pressButton = drawButton(startX, startY, nowText);
	if (pressButton) gameAdmitDefeat();
	
	outtextxy(startX, startY, nowText);
}

void gameAdmitDefeat()
{
	if (nowChessType == 'O') // ��������ǰ��������Ӯ��
	{
		gameResult = 2;
	}
	else
	{
		gameResult = 1;
	}
	running = false;

}

// �����ı�
void drawText()
{
	drawTextTitie(); // ��������ڵ�һ��Ϊ������ʼλ����������������õ�
	drawTextPicture();
	drawTextPrompt();
	drawTextStepBack();
	drawTextAdmitDefeat();
	drawTextWhiteTime();
	drawTextBlackTime();
	drawTextScore();
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
	if (!running) return;

	isClick = false;
	while (peekmessage(&msg))
	{
		if (msg.message == WM_MOUSEMOVE) // ����ƶ���Ϣ
		{
			mouseX = msg.x;
			mouseY = msg.y;
		}
		if (msg.message == WM_LBUTTONDOWN) {

			isClick = true;
		}

	}

}

// ��������Ƿ�Խ��
bool isOverBorder(int& row, int& y)
{
	if (row < 0 || row >= ROWS_WINDOW) return true;
	if (y < 0 || y >= COLS_WINDOW) return true;
	return false;
}

// ����Ƿ�ʤ��
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




// ����Ƿ����
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
	// ����ʱ��������
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
	// ÿ��һ��Ŀǰ����ʱ��һ
	if (time(0) - nowTimeSecondCD >= 1) {
		nowTimeSecondCD = time(0);
		--(*nowCDTime);
	}

	running = !gameTimeOver();
}


void chessBoardProcess() {
	if (!running) return;

	// ����Ƿ���
	if (!isClick) return;
	// �ж�����Ƿ�Խ������
	if (mouseX < START_CHECKERBOARD_X || mouseX > START_CHECKERBOARD_X + WIDTH_CHECKERBOARD - col_size ||
		mouseY < START_CHECKERBOARD_Y || mouseY > START_CHECKERBOARD_Y + HEIGHT_CHECKERBOARD - row_size) return;
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
	gameBoard[row][col] = nowChessType;

	// ��һ�����ӻ��Ƶ��к���
	lastChessPos.row = row;
	lastChessPos.col = col;

	chessCount++; // ���Ӹ�����һ
	running = !gameChessOver(row, col); // ����Ƿ����
	turnChessType();
}

// ���ݴ���
void gameProcess()
{

	// �������ݴ���
	chessBoardProcess();

	// ����ʱ����
	countdownProcess();

	

}

// ����֡��
void gamesleep()
{
	now_time = GetTickCount();
	DWORD delta_ms = now_time - start_time; // Ŀǰ�����˶���ʱ��
	DWORD frameMS = 1000 / GAME_FRAME; // һ֡���ٺ���
	if (delta_ms < frameMS)
	{
		Sleep(frameMS - delta_ms); // ���ߵ�һ֡��ʱ��
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



