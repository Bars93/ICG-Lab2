#include "WinProc.h"
WinApp *AppPtr = NULL;
LRESULT CALLBACK WndProcEXT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if(AppPtr == NULL) {
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	else {
		return AppPtr->WndProcINT(hWnd,msg,wParam,lParam);
	}
}
