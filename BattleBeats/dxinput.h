#include <dinput.h>
#include <Windows.h>

extern HINSTANCE hinst;				  //��ǰʵ��
extern HWND hwnd;                     //��ǰ���ھ��

bool InitDxInput();
void ReleaseDxInput();

bool KeyIsDown(int);
bool KeyIsClick(int);