#include "dxdraw.h"
#include "dxinput.h"
#include "music.h"

extern HINSTANCE hinst;				  //当前实例
extern HWND hwnd;                     //当前窗口句柄

namespace MyGame {
	void gameMain();
	void gameInit();
	void release();
}