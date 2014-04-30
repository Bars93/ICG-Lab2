#include <Windows.h> 
#include "WinApp.h"
#include "glCtrl.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{ 
	glModel model;
	glView view;
	glCtrl glctrl(&view,&model);
	WinApp glWin(800,600,&glctrl,L"ICG GL Lab2", L"ICG-GL-Lab2",false,0,hInstance);
	if(glWin.createWindow(WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,WS_EX_APPWINDOW)) {
		glWin.showWindow(SW_SHOWNORMAL);
		MSG msg;
		while(GetMessage(&msg, NULL, 0, 0) != NULL ) 
		{ 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		} 
		return msg.wParam;  
	}
	return -1;
} 
