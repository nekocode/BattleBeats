#include "dxImage.h"
#include <Windows.h>

LPDIRECT3DVERTEXBUFFER9 g_pVB;                  //顶点缓冲

bool InitVertex()
{
	//创建顶点缓存
	if( FAILED(g_pd3dD->CreateVertexBuffer(sizeof(Vertex)*4, 0, VertexFVF, D3DPOOL_MANAGED, &g_pVB, NULL)) )
		return false;

	Vertex* pVertices;
	if( FAILED(g_pVB->Lock( 0, 0, (void**)&pVertices, 0 )) )
		return false;

	pVertices[0] = Vertex(0.0f, 0.0f       ,       0.0f, 0.0f);
	pVertices[1] = Vertex(0.0f, 1.0f       ,       0.0f, 1.0f);
	pVertices[2] = Vertex(1.0f, 0.0f       ,       1.0f, 0.0f);
	pVertices[3] = Vertex(1.0f, 1.0f       ,       1.0f, 1.0f);
	g_pVB->Unlock();

	return true;
}

void ReleaseVertex()
{
	//释放顶点缓存
	if( g_pVB != NULL )
		g_pVB->Release();
}


//=============================================================
//                         dximg类
//=============================================================
dximg::dximg()
{
	g_pTexture = NULL;
	nowx = 0.0f;
	nowy = 0.0f;

	D3DXMatrixIdentity( &scale );
	D3DXMatrixIdentity( &trans );
	D3DXMatrixIdentity( &rot );
}

dximg::~dximg()
{
	if( g_pTexture != NULL )
		g_pTexture->Release();
}

bool dximg::loadimg(LPCTSTR pSrcFile)              //读取图片
{
	//获取图片信息
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFile(pSrcFile, &imageInfo);
	width = imageInfo.Width;
	height = imageInfo.Height;

	D3DXMatrixIdentity( &scale );
	D3DXMatrixIdentity( &trans );
	D3DXMatrixIdentity( &rot );

	if(FAILED(D3DXCreateTextureFromFileEx(g_pd3dD, pSrcFile, imageInfo.Width, imageInfo.Height,
		imageInfo.MipLevels, 0, imageInfo.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE,
		D3DX_DEFAULT, 0, NULL, NULL, &g_pTexture))) {
			return false;
	} else {
		setWH(width,height);
		return true;
	}
}

void dximg::setpos(float x,float y)                    //设置图片坐标
{
	this;
	nowx = (int)x; nowy = (int)y;
	D3DXMatrixTranslation(&trans, nowx-0.5f, nowy-0.5f, 0);
}

void dximg::move(float x,float y)                      //移动图片
{
	D3DXMatrixTranslation(&trans, (int)(nowx+x)-0.5f, (int)(nowy+y)-0.5f, 0); 
	nowx=nowx+x;
	nowy=nowy+y;
}

void dximg::setzoom(float zoomx,float zoomy)           //放缩图片
{
	width=width*zoomx;
	height=height*zoomy;
	D3DXMatrixScaling(&scale,width,height,0);
}

void dximg::setWH(float _width,float _height)         //设置图片长宽
{
	if(_width&&_height)
	{
		width = _width;
		height = _height;
	}
	D3DXMatrixScaling(&scale,width,height,0);
}

void dximg::setrota(float _rotation,float x,float y)
{
	D3DXMATRIXA16 tmp,tmp2,tmp3;
	rotation = _rotation;
	D3DXMatrixTranslation(&tmp, -x, -y, 0);
	D3DXMatrixRotationZ(&tmp2, rotation);
	D3DXMatrixTranslation(&tmp3, +x, +y, 0);
	rot = tmp*tmp2*tmp3;
}

void dximg::getmap() {
	map = new boolean *[width];
	for(unsigned int i=0;i<width;i++)
		map[i] = new byte[height];
	for(unsigned int x=0;x<width;x++) {
		for(unsigned int y=0;y<height;y++)
			map[x][y] = false;
	}

	D3DLOCKED_RECT locked_rect;
	g_pTexture->LockRect(0, &locked_rect, NULL, 0);

	BYTE *pByte = (BYTE *) locked_rect.pBits;

	for(unsigned int iRow=0; iRow<height; iRow++ ) {
		for(unsigned int iCol=0; iCol<width; iCol++ ) {
			if(pByte[3]>=128)
				map[iCol][iRow] = true;
			pByte += 4;
		}
	}

	g_pTexture->UnlockRect(0);
}

void dximg::draw(byte a)										//绘制图片
{
	g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));		//设置世界矩阵(rot)

	g_pd3dD->SetTexture( 0, g_pTexture );

	g_pd3dD->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255-a,0,0,0));
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	/*
	g_pd3dD->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);*/
	g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dD->SetStreamSource( 0, g_pVB, 0, sizeof( Vertex ) );
	g_pd3dD->SetFVF( VertexFVF );
	g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );

	//	g_pd3dD->SetTexture(0, NULL);
}

void dximg::drawAddColor(byte a, byte r, byte g, byte b, boolean light_render)	//绘制图片
{
	g_pd3dD->SetTransform(D3DTS_WORLD,&(scale*rot*trans));	//设置世界矩阵(rot)

	g_pd3dD->SetTexture( 0, g_pTexture );

	g_pd3dD->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255-a,r,g,b));
	g_pd3dD->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_ADD);
	g_pd3dD->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dD->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	///*
	if(light_render) {
		g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	} else {
		g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}//*/
	/*
	if(light_render) {
		g_pd3dD->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);

		g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dD->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
		g_pd3dD->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	} else {
		g_pd3dD->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);

		g_pd3dD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_pd3dD->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		g_pd3dD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}*/
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_SUBTRACT);		//相减
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dD->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	g_pd3dD->SetStreamSource( 0, g_pVB, 0, sizeof( Vertex ) );
	g_pd3dD->SetFVF( VertexFVF );
	g_pd3dD->DrawPrimitive( D3DPT_TRIANGLESTRIP ,0, 2 );
	
//	g_pd3dD->SetTexture(0, NULL);
}


