#include"hikfun.h"
//#include"utilies.h"
using namespace std;
using namespace cv;


HWND hWnd = NULL;

LONG g_lRealPlayHandle, g_lPort, g_lUserID;
int g_width = 1280, g_height = 720;

volatile bool g_bBusy = false;
char *g_pBuf = (char*)malloc(g_width*g_height * 3 / 2);


//����ص� ��ƵΪYUV����(YV12)����ƵΪPCM����
void CALLBACK DecCBFun(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nReserved1, long nReserved2)
{
	if (!g_bBusy)
	{
		g_bBusy = true;
		memcpy(g_pBuf, pBuf, g_width*g_height * 3 / 2);
		g_bBusy = false;
	}
	return ;
}


void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser)
{
	//hWnd = GetConsoleWindow();
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD: //ϵͳͷ


		if (!PlayM4_GetPort(&g_lPort))  //��ȡ���ſ�δʹ�õ�ͨ����
		{
			break;
		}
		//m_iPort = g_lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(g_lPort, STREAME_REALTIME))  //����ʵʱ������ģʽ
			{
				break;
			}

			if (!PlayM4_OpenStream(g_lPort, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
			{
				break;
			}

			//���ý���ص����� ֻ���벻��ʾ
			if (!PlayM4_SetDecCallBack(g_lPort, DecCBFun))
			{
				break;
			}

			if (!PlayM4_Play(g_lPort, hWnd)) //���ſ�ʼ
			{
				break;
			}
		}
		break;
	case NET_DVR_STREAMDATA:   //��������
		if (dwBufSize > 0 && g_lPort != -1)
		{
			// cout << "��������" << endl;
			if (!PlayM4_InputData(g_lPort, pBuffer, dwBufSize))
			{
				break;
			}
		}
		break;
	default: //��������
		if (dwBufSize > 0 && g_lPort != -1)
		{
			if (!PlayM4_InputData(g_lPort, pBuffer, dwBufSize))
			{
				break;
			}
		}
		break;
	}

}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT:    //Ԥ��ʱ����
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}

unsigned int __stdcall readCam(void *param)
{
	//---------------------------------------
	// ��ʼ��
	NET_DVR_Init();
	//��������ʱ��������ʱ��
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//---------------------------------------
	//�����쳣��Ϣ�ص�����
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

	/*/---------------------------------------
	// ��ȡ����̨���ھ��
	typedef HWND(WINAPI *PROCGETCONSOLEWINDOW)();
	PROCGETCONSOLEWINDOW GetConsoleWindow;
	HMODULE hKernel32 = GetModuleHandle("kernel32");
	GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");
	/**/
	//---------------------------------------
	// ע���豸

	//��¼�����������豸��ַ����¼�û��������
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //ͬ����¼��ʽ
	strcpy(struLoginInfo.sDeviceAddress, "192.168.2.254"); //�豸IP��ַ
	struLoginInfo.wPort = 8000; //�豸����˿�
	strcpy(struLoginInfo.sUserName, "admin"); //�豸��¼�û���
	strcpy(struLoginInfo.sPassword, "admin1234"); //�豸��¼����

	//�豸��Ϣ, �������
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	g_lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (g_lUserID < 0)
	{
		printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return 0;
	}

	//---------------------------------------
	//����Ԥ�������ûص�������

	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = hWnd;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo.lChannel = 1;       //Ԥ��ͨ����
	struPlayInfo.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo.dwLinkMode = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP

	g_lRealPlayHandle = NET_DVR_RealPlay_V40(g_lUserID, &struPlayInfo, g_RealDataCallBack_V30, NULL);
	if (g_lRealPlayHandle < 0)
	{
		printf("NET_DVR_RealPlay_V40 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(g_lUserID);
		NET_DVR_Cleanup();
		return 0;
	}
	getchar();
	return 0;
}

void freeHik(LONG lRealPlayHandle, LONG lPort, LONG lUserID)
{
	//---------------------------------------
	//�ر�Ԥ��
	NET_DVR_StopRealPlay(lRealPlayHandle);

	//�ͷŲ��ſ���Դ
	PlayM4_Stop(lPort);
	PlayM4_CloseStream(lPort);
	PlayM4_FreePort(lPort);

	//ע���û�
	NET_DVR_Logout(lUserID);
	NET_DVR_Cleanup();
	free(g_pBuf);
}

bool decodeImg(cv::Mat& dst)
{
	///cout << "decodeImg" << endl;
	if (g_bBusy)
		return false;
	//*
	g_bBusy = true;
	Mat src = cv::Mat(g_height * 3 / 2, g_width, CV_8UC1, g_pBuf);
	g_bBusy = false;

	cv::cvtColor(src, dst, CV_YUV2BGR_YV12);

	return true;
}
