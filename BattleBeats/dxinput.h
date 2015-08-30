#include <dinput.h>
#include <Windows.h>

extern HINSTANCE hinst;				  //当前实例
extern HWND hwnd;                     //当前窗口句柄

bool InitDxInput();
void ReleaseDxInput();

bool KeyIsDown(int);
bool KeyIsClick(int);