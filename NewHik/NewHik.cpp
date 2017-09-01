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
	//_beginthreadex(NULL, 0, &readCam, NULL, 0, NULL);
	//Sleep(5 * 1000);

	
	// -------------------------------- 1. �������� --------------------------------------- //
	/*
	while (waitKey(50) != 'q')
	{	
		
		waitKey(1);
	}
	/**/
	/*
	// --------------------------- 2. ���Ի��ã�����������Ϣ ----------------------------- //
	 drawMapLayout();
	 return 0;
	 /**/
	// ----------------------------- 3. �궨���������Ϣ -------------------------------- //
	int nrows = 13;
	int ncols = 9;
	int n_area = 117;
	int x_dis[117] = { 0 }; // �������ı������������Ӧ
	int y_dis[117] = { 0 };
	for (int i = 0; i < nrows; ++i)
	{
		for (int j = 0; j < ncols; ++j)
		{
			int npos = i *  ncols + j;
			x_dis[npos] = i;
			y_dis[npos] = j;
		}
	}
		
	 createDistanceMap(x_dis, y_dis,n_area);
	/**/
	
	return 0;
}



