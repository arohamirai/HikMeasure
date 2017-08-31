#ifndef HIKFUN_H
#define HIKFUN_H
#include <stdio.h>
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include <time.h>
#include "opencv2/opencv.hpp"

#include "process.h"

void on_mouse(int Event, int x, int y, int flags, void *userdata);

unsigned int __stdcall readCam(void *param);

void CALLBACK DecCBFun(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nReserved1, long nReserved2);

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser);

void freeHik(LONG lRealPlayHandle, LONG lPort, LONG lUserID);

bool decodeImg(cv::Mat& dst);


#endif