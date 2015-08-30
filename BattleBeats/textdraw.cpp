#include "textdraw.h"
#include "sleep.h"
#include <tchar.h>

#define textLevel 10		//10级缩放
CFreeTypeLib* g_pFreeTypeLib[textLevel];

extern IDirect3DDevice9* g_pd3dD;

bool initText() {
	//初始化字体共用顶点缓存
	if(!initFreeType(FontLibName))
		return false;
	return true;
}


void ReleaseText() {
	ReleaseFreeType();
}


//==================================
//  函数:drawtext(wchar_t *str,int x,int y,int font_width,int font_height,D3DCOLOR color,int line_width,bool center)
//  目的:绘制文本
//
void drawtext(wchar_t *str,int x,int y,int font_width,int font_height,D3DCOLOR color,int line_width,bool center) {
	int max = ((font_width>font_height?font_width:font_height)-1)/10+1;
	if(max > textLevel)
		max = textLevel;

	if(!g_pFreeTypeLib[max]) {
		g_pFreeTypeLib[max] = new CFreeTypeLib(g_pd3dD);
		g_pFreeTypeLib[max]->SetSize(max*10,max*10);
	}
	POINT point;
	point.x = x;
	point.y = y;
	g_pFreeTypeLib[max]->DrawText(str, point, font_width, font_height, line_width, center, color);
}