#pragma once
#include <d3dx9.h>

extern LPDIRECT3DDEVICE9 g_pd3dD;               //D3D设备，用于渲染

//用于单张图片显示
struct Vertex
{
	Vertex(){}
	Vertex(float _x, float _y,
		float _tu, float _tv)
	{
		x  = _x;  y  = _y;  z  = 0.0f;
		tu = _tu;  tv  = _tv;
	}
	FLOAT x, y, z; // The position
	FLOAT tu, tv;   // The texture coordinates
};
#define VertexFVF (D3DFVF_XYZ|D3DFVF_TEX1)


class dximg
{
public:
	LPDIRECT3DTEXTURE9 g_pTexture;             //纹理指针 
	dximg();
	~dximg();

	bool loadimg(LPCTSTR pSrcFile);
	void setpos(float x,float y);
	void move(float x,float y);
	void setzoom(float zoomx,float zoomy);
	void setWH(float width,float height);
	void setrota(float _rotation,float x,float y);
	void getmap();

	void draw(byte a=0xff);
	void drawAddColor(byte a=0xff, byte r=0xff, byte g=0xff, byte b=0xff, boolean light_render=false);

public:
	boolean** map;
	unsigned int width,height;
	float nowx,nowy;
	float rotation;

private:
	D3DXMATRIXA16      trans, rot, scale;
};


bool InitVertex();
void ReleaseVertex();
