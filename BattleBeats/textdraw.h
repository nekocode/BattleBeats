#include <d3dx9core.h>
#include "FreeTypeLib.h"

#define DefaultBufLen 256
#define FontLibName "res/default.ttf"

bool initText();
void ReleaseText();

void drawtext(wchar_t *str,int x,int y,int font_width=20,int font_height=20,D3DCOLOR color=0xffffffff,int line_width=800,bool center=false);