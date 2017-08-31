// NewHikDect.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include"iostream"
#include"opencv2/opencv.hpp"
#include "Windows.h"
#include"hikfun.h"
#include"vibe-background-sequential.h"

using namespace cv;
using namespace std;


extern int g_width, g_height;
extern volatile bool g_bBusy;
extern  char *g_pBuf;

extern Mat g_color;


int main()
{
	_beginthreadex(NULL, 0, &readCam, NULL, 0, NULL);
	Sleep(5 * 1000);
	// ----------------------------- 4. ���߼�� ---------------------------------------- //
	//*
	// ���   
	Mat distanceMap = imread("distanceMapImg.bmp", CV_LOAD_IMAGE_COLOR);
	Mat frame;
	Mat segmentationMap = Mat(g_height, g_width, CV_8UC1);;        // Will contain the segmentation map. This is the binary output map. 
	vibeModel_Sequential_t *model = (vibeModel_Sequential_t*)libvibeModel_Sequential_New();

	// 

	// ��ʼ��
	while (1)
	{
		bool ret = decodeImg(frame);
		if (!ret) continue;
		libvibeModel_Sequential_AllocInit_8u_C3R(model, frame.data, frame.cols, frame.rows);
		break;
	}
	// ���
	VideoWriter writer("FindPersonNoFilt.avi", CV_FOURCC('M', 'J', 'P', 'G'), 15, Size(g_width, g_height), true);
	while (waitKey(1) != 27) //ESC
	{
		bool ret = decodeImg(frame);
		if (!ret) continue;
		libvibeModel_Sequential_Segmentation_8u_C3R(model, frame.data, segmentationMap.data);
		libvibeModel_Sequential_Update_8u_C3R(model, frame.data, segmentationMap.data);

		//medianBlur(segmentationMap, segmentationMap, 3); /* 3x3 median filtering */

		morphologyEx(segmentationMap, segmentationMap, MORPH_OPEN, cv::Mat(3, 3, CV_8UC1, cv::Scalar(1))); // �˵����
		morphologyEx(segmentationMap, segmentationMap, MORPH_CLOSE, cv::Mat(17, 17, CV_8UC1, cv::Scalar(1)));// ��ͨ��������

		// ��ͨ�����
		double areaThresh = 50;
		vector<vector<Point> > contours, contours1;
		vector<Vec4i> hierarchy;
		vector<Rect> vecRect;
		findContours(segmentationMap, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		// Ŀ�궨λ
		// �޳������С������,����С��Ӿ���
		if (contours.size() == 0) continue;
		for (int i = 0; i<contours.size(); i++)
		{
			double tmpArea = fabs(contourArea(contours[i]));
			if (tmpArea < areaThresh)
				continue;

			contours1.push_back(contours[i]);
			Rect rect = (minAreaRect(contours[i])).boundingRect();
			vecRect.push_back(rect);
			rectangle(frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), CV_RGB(rand() % 255, rand() % 255, rand() % 255), 3);
			//line(frame, cv::Point(rect.x, rect.y + rect.height), cv::Point(rect.x + rect.width, rect.y + rect.height), CV_RGB(0, 0, 0), 5);

			cv::Point pt;// ��������ıߵ��е�
			pt.x = rect.x + rect.width / 2;
			pt.y = rect.y + rect.height;

			circle(frame, pt, 5, CV_RGB(255, 0, 0), CV_FILLED);
			// ---------------------- ��ѯ���� --------------------- //
			//cout << "distance = " << distanceMap.at<char>(pt.x, pt.y) << endl;
		}
		imshow("frame", frame);
	}
	libvibeModel_Sequential_Free(model);
	getchar();
}

