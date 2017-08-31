#include"createDistanceMap.h"

using namespace std;
using namespace cv;

extern int g_width, g_height;
extern  char *g_pBuf;
cv::Mat g_color;
cv::Point g_p1, g_p2;
std::vector<LINE> g_l;
std::vector<std::vector<Point>> g_vecRect;

int scale = 1;   // 当初显示器尺寸不够，导致引入此缩放参数:(

// -----------------  需要保存的东西 -------------------- //
cv::Mat g_lineImg = Mat(cv::Size(g_width, g_height), CV_8UC1, CV_RGB(0, 0, 0));
cv::Mat g_distanceImg = Mat(cv::Size(g_width, g_height), CV_8UC1, CV_RGB(0, 0, 0));


void on_mouse(int Event, int x, int y, int flags, void *userdata)
{
	
	bool ret = decodeImg(g_color);
	if (!ret) return;
	imshow("pic", g_color);

	cv::Vec3i p(0, 0, 255);
	g_color.setTo(p, g_lineImg);
	imshow("pic", g_color);

	if (Event == CV_EVENT_LBUTTONDOWN){
		g_p1 = cvPoint(x, y);
	}
	if (Event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON){
		line(g_color, g_p1, Point(x, y), CV_RGB(0, 255, 0));
		OnDrawDotline(g_p1, Point(x, y), g_color);
		imshow("pic", g_color);
	}
	if (Event == CV_EVENT_LBUTTONUP){
		g_p2 = cvPoint(x, y);
		cv::line(g_lineImg, g_p1, g_p2, CV_RGB(255, 255, 255));
		OnDrawDotline(g_p1, Point(x, y), g_lineImg);
		g_l.push_back(getDotlinePoint(g_p1, g_p2, g_color.size()));
	}

	g_color.setTo(p, g_lineImg);
	imshow("pic", g_color);
	waitKey(1);
}

void  drawMapLayout()
{
	//----- 保存所划直线 ------
	imwrite("line.bmp", g_lineImg);
	// ----- 保存实际区域被所划直线的分割情形 ------
	Mat color;
	bool ret = false;
	while (!ret)
		ret = decodeImg(color);
	cv::Vec3i p(0, 0, 255);
	color.setTo(p, g_lineImg);
	imwrite("colorLine.bmp", color);

	//添加四条外框直线
	LINE r0, r1, c0, c1;
	r0.pStart = Point(0, 0);
	r0.pEnd = Point(g_width / scale, 0);

	r1.pStart = Point(0, g_height / scale);
	r1.pEnd = Point(g_width / scale, g_height / scale);

	c0.pStart = Point(0, 0);
	c0.pEnd = Point(0, g_height / scale);

	c1.pStart = Point(g_width / scale, 0);
	c1.pEnd = Point(g_width / scale, g_height / scale);

	line(g_lineImg, r0.pStart, r0.pEnd, CV_RGB(255, 255, 255), 1);
	line(g_lineImg, r1.pStart, r1.pEnd, CV_RGB(255, 255, 255), 1);
	line(g_lineImg, c0.pStart, c0.pEnd, CV_RGB(255, 255, 255), 1);
	line(g_lineImg, c1.pStart, c1.pEnd, CV_RGB(255, 255, 255), 1);

	g_l.push_back(r0);
	g_l.push_back(r1);
	g_l.push_back(c0);
	g_l.push_back(c1);

	//行和列直线分开
	std::vector<LINE> r_l, c_l;
	for (int i = 0; i < g_l.size(); i++){
	
		if ((g_l[i].pStart.x == 0 && g_l[i].pEnd.x == g_width / scale) ||
			(g_l[i].pEnd.x == 0 && g_l[i].pStart.x == g_width / scale)
			)
		{
			r_l.push_back(g_l[i]);
		}

		else if ((g_l[i].pStart.y == 0 && g_l[i].pEnd.y == g_height / scale) ||
			(g_l[i].pEnd.y == 0 && g_l[i].pStart.y == g_height / scale)
			)
		{
			c_l.push_back(g_l[i]);
		}
		/*if (g_l[i].pStart.x == 0 && g_l[i].pEnd.x != 0)
			r_l.push_back(g_l[i]);
		else if (g_l[i].pStart.y == 0 && g_l[i].pEnd.y != 0)
			c_l.push_back(g_l[i]);
			*/
	}
	//排序直线
	// 行排列
	for (int i = 0; i < r_l.size() - 1; i++)
	{
		for (int j = i + 1; j < r_l.size(); j++)
		{
			LINE lt;
			if (r_l[i].pStart.y > r_l[j].pStart.y)
			{
				lt = r_l[i];
				r_l[i] = r_l[j];
				r_l[j] = lt;
			}
		}
	}
	// 列排列
	for (int i = 0; i < c_l.size() - 1; i++)
	{
		for (int j = i + 1; j < c_l.size(); j++)
		{
			LINE lt;
			if (c_l[i].pStart.x > c_l[j].pStart.x)
			{
				lt = c_l[i];
				c_l[i] = c_l[j];
				c_l[j] = lt;
			}
		}
	}
	// ---------- 分别画出行直线和列直线，看是不是正确的 -------------
	Mat rowLine = g_color.clone(), colLine = g_color.clone();
	for (int i = 0; i < r_l.size(); i++)
	{
		line(rowLine, r_l[i].pStart, r_l[i].pEnd, CV_RGB(0, 0, 255));
	}
	for (int i = 0; i < c_l.size(); i++)
	{
		line(colLine, c_l[i].pStart, c_l[i].pEnd, CV_RGB(0, 0, 255));
	}
	imshow("rowLine", rowLine);
	imshow("colLine", colLine);
	//waitKey(0);

	// -------------- 求直线之间的交点 ---------------------
	std::vector<std::vector<cv::Point>> vecCP_r;
	for (int i = 0; i < r_l.size(); i++)
	{
		std::vector<cv::Point> vecCP;
		for (int j = 0; j <c_l.size(); j++)
		{
			Point p = CrossPoint(&r_l[i], &c_l[j]);
			vecCP.push_back(p);
		}
		vecCP_r.push_back(vecCP);

	}

	// ---------------------  画出交点，看是不是正确 ------------ //
	int n = 0;
	for (int i = 0; i < vecCP_r.size(); i++)
	{
		for (int j = 0; j < vecCP_r[i].size(); j++)
		{
			char text[5];
			sprintf(text, "%d", n++);
			putText(g_color, text, vecCP_r[i][j], 1, 1, CV_RGB(255, 255, 255));
		}

	}
	imshow("crosspoint", g_color);
	imwrite("crosspoint.jpg", g_color);
	waitKey(1000);

	// --------------- 组合交点成四边形区域 ----------------------- //
	std::vector<std::vector<Point>> vecRect;
	for (int i = 0; i < vecCP_r.size() - 1; i++)
	{
		for (int j = 0; j < vecCP_r[i].size() - 1; j++)
		{
			vector<Point> vec;
			vec.push_back(vecCP_r[i][j]);
			vec.push_back(vecCP_r[i][j + 1]);
			vec.push_back(vecCP_r[i + 1][j + 1]);
			vec.push_back(vecCP_r[i + 1][j]);

			vecRect.push_back(vec);
		}
	}

	//*
	// ------------------ 保存四边形 ---------------------- //
	FILE *prect = fopen("rect.dat", "w");
	fprintf(prect, "%d\n", vecRect.size());
	fprintf(prect, "%d %d\n", g_width / scale, g_height / scale);
	for (int i = 0; i < vecRect.size(); i++)
	{
		fprintf(prect, "%d %d %d %d %d %d %d %d\n", vecRect[i][0].x, vecRect[i][0].y, vecRect[i][1].x, vecRect[i][1].y ,
			vecRect[i][2].x, vecRect[i][2].y, vecRect[i][3].x, vecRect[i][3].y);
	}
	fclose(prect);
	/**/

	//*
	// --------------------- 显示分区域是否正确 ------------------------ //
	Mat distanceMapColor;
	distanceMapColor.create(Size(g_width / scale, g_height / scale), CV_8UC3);
	for (int i = 0; i < vecRect.size(); i++)
	{
		auto vec = vecRect[i];
		int num = (int)vec.size();
		const Point *pt = &(vec[0]);

		cv::Scalar color = CV_RGB(rand() % 255, rand() % 255, rand() % 255);
		fillPoly(distanceMapColor, &pt, &num, 1, color);
	}
	imshow("distanceMapColor", distanceMapColor);
	imwrite("distanceMapColor.jpg", distanceMapColor);
	waitKey(1);
	/**/


	// 保存距离映射图像
	//*
	Mat distanceMap;
	distanceMap.create(Size(g_width / scale, g_height / scale), CV_8UC1);
	

	int stepColor =  255 / (int)vecRect.size();
	for (int i = 0; i < vecRect.size(); i++)
	{
		auto vec = vecRect[i];
		int num = (int)vec.size();
		const Point *pt = &(vec[0]);

		cv::Scalar color = CV_RGB(stepColor*i, stepColor*i, stepColor*i);
		fillPoly(distanceMap, &pt, &num, 1, color);
	}
	imwrite("distanceMap.bmp", distanceMap);
	/**/
}

void loadDistanceInfo()
{
	// --------------------- 读入四边形区域 ------------------------ //
	int num = 0;
	int width = 0, height = 0;

	FILE *pRect = fopen("rect.dat", "r");
	fscanf(pRect, "%d\n", &num);
	fscanf(pRect, "%d %d\n", &width, &height);
	for (int i = 0; i < num; i++)
	{
		cv::Point p0, p1, p2, p3;
		fscanf(pRect, "%d %d %d %d %d %d %d %d\n", &p0.x, &p0.y, &p1.x, &p1.y,
			&p2.x, &p2.y, &p3.x, &p3.y);
		std::vector<Point> vRect;
		vRect.push_back(p0);
		vRect.push_back(p1);
		vRect.push_back(p2);
		vRect.push_back(p3);
		g_vecRect.push_back(vRect);
	}
	fclose(pRect);

	g_lineImg = imread("line.bmp", CV_LOAD_IMAGE_GRAYSCALE);
}
void createDistanceMap(int x_dis[],int y_dis[],int n_area)
{
	loadDistanceInfo();
	Mat distanceMapImg(Size(g_width / scale, g_height / scale), CV_8UC3);
	
	for (int i = 0; i < g_vecRect.size(); i++)
	{
		auto vec = g_vecRect[i];
		int num = (int)vec.size();
		const Point *pt = &(vec[0]);

		cv::Scalar color = CV_RGB(x_dis[i], y_dis[i], 0);
		fillPoly(distanceMapImg, &pt, &num, 1, color);
	}
	imwrite("distanceMapImg.bmp", distanceMapImg);
}

unsigned int __stdcall showRealPlayWithArea(void *param)
{
	while(1)
	{
		bool ret = decodeImg(g_color);
		if (!ret) continue;

		cv::Vec3i p(0, 0, 255);
		g_color.setTo(p, g_lineImg);
		imshow("realPlay_Area", g_color);
		waitKey(1);
	}
	return 0;
}


cv::Point CrossPoint(const LINE *line1, const LINE *line2)
{
	//	if(!SegmentIntersect(line1->pStart, line1->pEnd, line2->pStart, line2->pEnd))
	//	{// segments not cross	
	//		return 0;
	//	}
	cv::Point pt;
	// line1's cpmponent
	double X1 = line1->pEnd.x - line1->pStart.x;//b1
	double Y1 = line1->pEnd.y - line1->pStart.y;//a1
	// line2's cpmponent
	double X2 = line2->pEnd.x - line2->pStart.x;//b2
	double Y2 = line2->pEnd.y - line2->pStart.y;//a2
	// distance of 1,2
	double X21 = line2->pStart.x - line1->pStart.x;
	double Y21 = line2->pStart.y - line1->pStart.y;
	// determinant
	double D = Y1*X2 - Y2*X1;// a1b2-a2b1
	// 
	if (D == 0) return 0;
	// cross point
	pt.x = (X1*X2*Y21 + Y1*X2*line1->pStart.x - Y2*X1*line2->pStart.x) / D;
	// on screen y is down increased ! 
	pt.y = -(Y1*Y2*X21 + X1*Y2*line1->pStart.y - X2*Y1*line2->pStart.y) / D;
	// segments intersect.
	if ((abs(pt.x - line1->pStart.x - X1 / 2) <= abs(X1 / 2)) &&
		(abs(pt.y - line1->pStart.y - Y1 / 2) <= abs(Y1 / 2)) &&
		(abs(pt.x - line2->pStart.x - X2 / 2) <= abs(X2 / 2)) &&
		(abs(pt.y - line2->pStart.y - Y2 / 2) <= abs(Y2 / 2)))
	{
		return pt;
	}
	return 0;
}

LINE getDotlinePoint(cv::Point _s, cv::Point _e, cv::Size sz)
{
	cv::Point s, e;
	if (_s.x <= _e.x)
	{
		s = _s;
		e = _e;
	}
	else
	{
		s = _e;
		e = _s;
	}

	CvPoint pa, pb;

	double k = (s.y - e.y) / (s.x - e.x + 0.000001);//不加0.000001 会变成曲线，斜率可能为0，即e.x-s.x可能为0  

	double h = sz.height, w = sz.width;

	pa.y = s.y - k*s.x;
	if (pa.y < 0)
	{
		pa.y = 0;
		pa.x = s.x - s.y / k;
	}
	else if (pa.y > h)
	{
		pa.y = h;
		pa.x = s.x + (h - s.y) / k;
	}

	else
	{
		pa.x = 0;
	}

	//y = kx +b 
	pb.y = e.y + k*(w - e.x);
	if (pb.y < 0)
	{
		pb.y = 0;
		pb.x = e.x - e.y / k;
	}
	else if (pb.y > h)
	{
		pb.y = h;
		pb.x = e.x + (h - e.y) / k;
	}
	else
	{
		pb.x = w;
	}




	LINE l;
	l.pStart = pa;
	l.pEnd = pb;



	return l;
	//line(workimg, e, pa, CV_RGB(0, 255, 255));   //向右画线  
	//line(workimg, pb, s, CV_RGB(0, 0, 255)); //向左画线  

}

void OnDrawDotline(cv::Point _s, cv::Point _e, cv::Mat &workimg)
{
	cv::Point s, e;
	if (_s.x <= _e.x)
	{
		s = _s;
		e = _e;
	}
	else
	{
		s = _e;
		e = _s;
	}

	CvPoint pa, pb;

	double k = (s.y - e.y) / (s.x - e.x + 0.000001);//不加0.000001 会变成曲线，斜率可能为0，即e.x-s.x可能为0  

	double h = workimg.rows, w = workimg.cols;

	/*
	pa.x = w;
	pa.y = s.y + k*(w - s.x);
	pb.y = (e.y - k*e.x);
	pb.x = 0;
	*/

	pa.y = s.y - k*s.x;
	if (pa.y < 0)
	{
		pa.y = 0;
		pa.x = s.x - s.y / k;
	}
	else if (pa.y > h)
	{
		pa.y = h;
		pa.x = s.x + (h - s.y) / k;
	}

	else
	{
		pa.x = 0;
	}

	//y = kx +b 
	pb.y = e.y + k*(w - e.x);
	if (pb.y < 0)
	{
		pb.y = 0;
		pb.x = e.x - e.y / k;
	}
	else if (pb.y > h)
	{
		pb.y = h;
		pb.x = e.x + (h - e.y) / k;
	}
	else
	{
		pb.x = w;
	}

	line(workimg, pa, s, CV_RGB(255, 255, 255));   //向右画线 
	line(workimg, e, pb, CV_RGB(255, 255, 255)); //向左画线  

	//printf("inline = %d,%d,%d,%d\n", pa.x, pa.y, pb.x, pb.y);
}