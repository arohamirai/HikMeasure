// NewHik.cpp : �������̨Ӧ�ó������ڵ㡣
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


	/// ------------------ 0. �����ܷ��ȡͼ��,����ͨ����ע�͵��ô��� ---------------------- //
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
	
	// -------------------------------- 1. �������� --------------------------------------- //
	//*
	while (waitKey(50) != 'q')
	{	
		
		waitKey(1);
	}
	/**/
	//*
	// --------------------------- 2. ���Ի��ã�����������Ϣ ----------------------------- //
	 drawMapLayout();
	// ----------------------------- 3. �궨���������Ϣ -------------------------------- //
	 int n_area = 8;
	 int x_dis[8] = { 0 }; // �������ı������������Ӧ
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



