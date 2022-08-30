#include "chessai.h"
#include<graphics.h>
#include<windows.h>


bool start_view()//加载初始界面
{
	IMAGE img;
	loadimage(&img, _T("img\\start_view.png"), 800, 400, true);
	putimage(0, 0, &img);
	loadimage(&img, _T("img\\title.png"), 320, 120, true);
	putimage(50, 20, &img);
	loadimage(&img, _T("img\\start.png"), 170, 80, true);
	putimage(80, 180, &img);
	loadimage(&img, _T("img\\exit_1.png"), 170, 80, true);
	putimage(80, 280, &img);
	MOUSEMSG m;		// 定义鼠标消息
	while (true)
	{
		// 获取一条鼠标消息
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONUP)
		{
			if (m.x >= 80 && m.x <= 250 && m.y >= 180 && m.y <= 260)
				return true;
			else if (m.x >= 80 && m.x <= 250 && m.y >= 280 && m.y <= 360)
				return false;
		}
	}
}

void init_play_view()//初始化游戏界面
{
	cleardevice();// 用背景色清空屏幕
	IMAGE img;
	loadimage(&img, _T("img\\play_view.png"), 800, 400, true);
	putimage(0, 0, &img);
	loadimage(&img, _T("img\\regret.png"), 112, 40, true);
	putimage(20, 20, &img);
	loadimage(&img, _T("img\\exit_2.png"), 112, 40, true);
	putimage(20, 100, &img);
	loadimage(&img, _T("img\\chess_board.png"), 300, 300, true);
	putimage(350, 50, &img);
	setlinecolor(BLACK);
	for (int i = 0; i < 15; i++)//画网格线
	{
		line(350 + 10, 50 + 10 + i * 20, 350 + 290, 50 + 10 + i * 20);
		line(350 + 10 + 20 * i, 50 + 10, 350 + 10 + 20 * i, 50 + 290);
	}
}

bool end_view(int result)//加载结束界面
{
	Sleep(1000);
	IMAGE img;
	loadimage(&img, _T("img\\end_view.png"), 800, 400, true);
	putimage(0, 0, &img);
	switch (result)
	{
	case R_BLACK:
		loadimage(&img, _T("img\\victory.png"), 360, 60, true);
		putimage(100,70, &img);
		break;
	case R_WHITE:
		loadimage(&img, _T("img\\loss.png"), 360, 60, true);
		putimage(100, 100, &img);
		break;
	case R_DRAW:
		loadimage(&img, _T("img\\equal.png"), 360, 60, true);
		putimage(100, 100, &img);
		break;
	default:
		;
	}
	loadimage(&img, _T("img\\again.png"), 120, 40, true);
	putimage(150, 300, &img);
	loadimage(&img, _T("img\\exit_3.png"), 120, 40, true);
	putimage(300, 300, &img);
	MOUSEMSG m;		// 定义鼠标消息
	while (true)
	{
		// 获取一条鼠标消息
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONUP)
		{
			if (m.x >= 150 && m.x <= 270 && m.y >= 300 && m.y <= 340)
				return true;
			else if (m.x >= 300 && m.x <= 420 && m.y >= 300 && m.y <= 340)
				return false;
		}
	}
}

int main()
{
	initgraph(800, 400);//打开300*300大小的画布
	bool start_or_not = start_view();
	if (start_or_not)
	{
		while (1)
		{
			init_play_view();
			Play my_play;
			int result = my_play.game();//后续可以改为多轮游戏
			bool again_or_not=end_view(result);
			if (again_or_not)
				continue;
			else
				break;
		}
		closegraph();//关闭画布
	}
	return 0;
}