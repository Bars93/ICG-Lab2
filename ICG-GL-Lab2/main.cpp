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
	glWin.createWindow(WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,NULL);
	glWin.showWindow(SW_SHOWDEFAULT);
	MSG msg;

	while(TRUE) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) // verify messages in order
		{ 
			if (GetMessage(&msg, NULL, 0, 0) ) 
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			} else { 
				return msg.wParam; 
			} 
		} 

	}
	return 0;  
} 
