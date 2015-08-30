#include "win.h"

// �˴���ģ���а����ĺ�����ǰ������:
ATOM myRegisterClass(HINSTANCE hInstance);
BOOL initInstance(HINSTANCE, int);
LRESULT CALLBACK msgProc(HWND, UINT, WPARAM, LPARAM);
void setCurrentPath();

INT WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, INT ) {
	UNREFERENCED_PARAMETER(hInstance);

	// ע�ᴰ����
	myRegisterClass(hInstance);
	// ����������
	if (!initInstance (hInstance, SW_SHOWDEFAULT))
		return FALSE;

	GetClientRect(hwnd,&rect_client);
	//��ʼ��D3D
	MyGame::gameInit();

	// ����Ϣѭ��:
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

// ע�ᴰ���ࡣ
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
	wcex.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);         //������ɫ:��ɫ
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

// Ŀ��: ����ʵ�����������������
BOOL initInstance(HINSTANCE hInstance, int nCmdShow) {
	hinst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	RECT rect;
	rect.left=0;
	rect.top=0;
	rect.right=640;
	rect.bottom=400;
	//��������ͻ����εĴ�С��������Ҫ�Ĵ��ھ��εĴ�С(false=û�в˵�)
	AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,false);

	hwnd = CreateWindowEx( NULL,
		szWindowClass,						//��������
		szTitle,							//���ڱ���
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,						//��ʼxλ��
		CW_USEDEFAULT,						//��ʼyλ��
		rect.right-rect.left,				//��ʼ���
		rect.bottom-rect.top,				//��ʼ�߶�
		NULL,
		NULL,								//���ڲ˵����
		hInstance,							//ʵ�����
		NULL );

	if(!hwnd)  
		return FALSE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	setCurrentPath();

	return TRUE;
}

// Ŀ��: ���������ڵ���Ϣ��
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
	//���exe����Ŀ¼
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
	//����exe����Ŀ¼Ϊ��ǰĿ¼
	SetCurrentDirectory(current_path);
}