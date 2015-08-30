#include "dxinput.h"

IDirectInput8 *dinput;
IDirectInputDevice8 *dinputDK;          //KeyboardDevice

char ks[256];    //储存键盘按键状态

//=================================================================================
//=================================================================================
//=================================================================================
//=================================================================================

bool InitDxInput()
{
	if(FAILED(DirectInput8Create(hinst,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&dinput,NULL)))
		return false;

	//初始化keyboaard对象
	if(FAILED(dinput->CreateDevice(GUID_SysKeyboard,&dinputDK,NULL)))
		return false;
	if(FAILED(dinputDK->SetCooperativeLevel(hwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))    //窗口活动下获取&&非独占
		return false;
	if(FAILED(dinputDK->SetDataFormat(&c_dfDIKeyboard)))
		return false;
	dinputDK->Acquire();


	return true;
}

void ReleaseDxInput()
{
	dinputDK->Unacquire();

	if( dinputDK != NULL )
        dinputDK->Release();

    if( dinput != NULL )
        dinput->Release();
}

bool KeyIsDown(int iKey)
{
	HRESULT hr;

	hr = dinputDK->Poll();
    if( FAILED( hr ) )
    {
        hr = dinputDK->Acquire();
        while( hr == DIERR_INPUTLOST )
            hr = dinputDK->Acquire();
        return S_OK;
    }

	dinputDK->GetDeviceState( sizeof(ks), (LPVOID)ks );
	 
	if( ks[iKey] & 0x80)
		return true;
	
	return false;
}

bool KeyIsClick(int iKey)
{
	static int olds[256];       //旧状态
	if((olds[iKey]==0))
	{
		if(KeyIsDown(iKey))
		{
			olds[iKey]=1;
			return true;
		}
		else
			return false;
	}
	else
	{
		if(!KeyIsDown(iKey))
			olds[iKey]=0;
		return false;
	}
}