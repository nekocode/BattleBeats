#include <Windows.h>
#include <tchar.h>
#include "gameMain.h"

//ȫ�ֱ���
HINSTANCE hinst;			// ��ǰʵ��
HWND hwnd;					// ��ǰ���ھ��
RECT rect_client;			//�ͻ�����ǰ����
int mouse_x,mouse_y;


LPCWSTR szTitle=L"BattleBeat(ս������)";			// �������ı�
LPCWSTR szWindowClass=L"BattleBeat";			// ����������