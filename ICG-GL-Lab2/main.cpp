#include <Windows.h> 
#include "IrbisGLminiFW.h"


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR, int) 
{ 
	const int wWidth = 800;
	const int wHeight = 600;
	WinApp App(wWidth,wHeight,L"ICG GL Lab2", L"ICG-GL-Lab2",hInstance);
	AppPtr = &App; // Should init global ptr for WndProc connection
	MSG msg = {0};
	if(App.InitMutex() && App.createWindow()) {
		App.showWindow(SW_SHOWNORMAL);
		App.mainLoop(&msg);
	}
	else {
		msg.wParam = -1;
	}
	return msg.wParam;
} 
