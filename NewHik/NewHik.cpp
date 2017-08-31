// NewHik.cpp : 定义控制台应用程序的入口点。
//

#include"iostream"
#include"opencv2/opencv.hpp"
#include "Windows.h"
#include"hikfun.h"
#include"createDistanceMap.h"
#include"vibe-background-sequential.h"

using namespace cv;
using namespace std;


extern int g_width, g_height;
extern volatile bool g_bBusy;
extern  char *g_pBuf;
extern Mat g_color;


int main()
{
	namedWindow("pic", 1);
	setMouseCallback("pic", on_mouse);
	_beginthreadex(NULL, 0, &readCam, NULL, 0, NULL);
	Sleep(5 * 1000);


	/// ------------------ 0. 测试能否获取图像,测试通过后注释掉该代码 ---------------------- //
	/*
	while (true)
	{
		Mat color;
		bool ret = decodeImg(color);
		if (ret)
		{
			imshow("color", color);
			waitKey(1);
		}
	}
	/**/
	
	// -------------------------------- 1. 划分区域 --------------------------------------- //
	//*
	while (waitKey(50) != 'q')
	{	
		
		waitKey(1);
	}
	/**/
	//*
	// --------------------------- 2. 线以划好，导出区域信息 ----------------------------- //
	 drawMapLayout();
	// ----------------------------- 3. 标定区域距离信息 -------------------------------- //
	 int n_area = 8;
	 int x_dis[8] = { 0 }; // 个数与四边形区域个数对应
	 int y_dis[8] = { 0 };
	 for (int i = 0; i < 56; ++i)
	 {
		 x_dis[i] = rand() % 255;
		 y_dis[i] = rand() % 255;
	 }
	 createDistanceMap(x_dis, y_dis,n_area);
	/**/
	
	return 0;
}



