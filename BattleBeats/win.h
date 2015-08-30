#include <Windows.h>
#include <tchar.h>
#include "gameMain.h"

//全局变量
HINSTANCE hinst;			// 当前实例
HWND hwnd;					// 当前窗口句柄
RECT rect_client;			//客户区当前矩形
int mouse_x,mouse_y;


LPCWSTR szTitle=L"BattleBeat(战斗节奏)";			// 标题栏文本
LPCWSTR szWindowClass=L"BattleBeat";			// 主窗口类名