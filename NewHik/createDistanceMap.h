#ifndef CREATEDISTANCEMAP_H
#define  CREATEDISTANCEMAP_H

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include"hikfun.h"

//-------------------------------------
struct LINE
{
	cv::Point pStart;
	cv::Point pEnd;

	bool operator==(LINE l)
	{
		if (l.pStart.x == pStart.x && l.pStart.y == pStart.y && l.pEnd.x == pEnd.x && l.pEnd.y == pEnd.y)
			return true;
		else
			return false;
	};
};

//----------------------------------------------
void on_mouse(int Event, int x, int y, int flags, void *userdata);

void drawMapLayout();
void createDistanceMap(int x_dis[], int y_dis[], int n_area);
void loadDistanceInfo();

unsigned int __stdcall showRealPlayWithArea(void *param);

cv::Point CrossPoint(const LINE *line1, const LINE *line2);
LINE getDotlinePoint(cv::Point _s, cv::Point _e, cv::Size sz);
std::vector<cv::Point> softPoint(std::vector<cv::Point> vecIn);
void OnDrawDotline(cv::Point s, cv::Point e, cv::Mat &workimg);

static int cmp_func(const void* _a, const void* _b, void* userdata)
{
	CvPoint* a = (CvPoint*)_a;
	CvPoint* b = (CvPoint*)_b;
	int y_diff = a->y - b->y;
	int x_diff = a->x - b->x;
	//return x_diff ? x_diff : 0;
	return y_diff ? y_diff : x_diff;//优先比交y坐标，a的y坐标大则调整a，b位置;否则比较x坐标，a的x坐标大，也调整a，b位置。
} //即交换a,b位置 swap(a,b) if (a->y >b->y)|(a->x >b->x)

#endif