#include "win.h"

// 此代码模块中包含的函数的前向声明:
ATOM myRegisterClass(HINSTANCE hInstance);
BOOL initInstance(HINSTANCE, int);
LRESULT CALLBACK msgProc(HWND, UINT, WPARAM, LPARAM);
void setCurrentPath();

INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT ) {
	UNREFERENCED_PARAMETER(hInstance);

	// 注册窗口类
	myRegisterClass(hInstance);
	// 创建主窗口
	if (!initInstance (hInstance, SW_SHOWDEFAULT))
		return FALSE;

	GetClientRect(hwnd,&rect_client);
	//初始化D3D
	MyGame::gameInit();

	// 主消息循环:
	MSG msg;
	ZeroMemory(&msg,sizeof(msg));
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		MyGame::gameMain();
	}

	MyGame::release();

	UnregisterClass(szWindowClass,hinst);
	return 0;
}

// 注册窗口类。
ATOM myRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= msgProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //背景颜色:黑色
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

// 目的: 保存实例句柄并创建主窗口
BOOL initInstance(HINSTANCE hInstance, int nCmdShow) {
	hinst = hInstance; // 将实例句柄存储在全局变量中

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=640;
	rect.bottom=400;
	//依据所需客户矩形的大小，计算需要的窗口矩形的大小(false=没有菜单)
	AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,false);

	hwnd = CreateWindowEx( NULL,
		szWindowClass,						//窗口类名
		szTitle,							//窗口标题
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,						//初始x位置
		CW_USEDEFAULT,						//初始y位置
		rect.right-rect.left,				//初始宽度
		rect.bottom-rect.top,				//初始高度
		NULL,
		NULL,								//窗口菜单句柄
		hInstance,							//实例句柄
		NULL );

	if(!hwnd)  
		return FALSE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	setCurrentPath();

	return TRUE;
}

// 目的: 处理主窗口的消息。
LRESULT CALLBACK msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hwnd,&rect_client);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void setCurrentPath() {
	//获得exe所在目录
	wchar_t buf_path[MAX_PATH];
	wchar_t current_path[MAX_PATH];
	GetModuleFileName(NULL, current_path, MAX_PATH);
	GetLongPathName(buf_path, current_path, MAX_PATH);
	for(int i = wcslen(current_path); i>=0; i--) {
		if(current_path[i] == '\\') {
			current_path[i+1] = '\0';
			break;
		}
	}
	//设置exe所在目录为当前目录
	SetCurrentDirectory(current_path);
}