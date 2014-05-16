#pragma warning(disable : 4996)
#ifdef _WIN32 // Windows only
#ifndef WINAPP_H
#define WINAPP_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> 
#include <windowsx.h> // Extended windows defenisions
#include <CommCtrl.h>
#include <wchar.h> // for UNICODE characters functions
#include "glCtrl.h"
class WinApp
{
	bool fullscreen;
	int width, height;
	int x,y;
	HWND handle, hBtnState;
	// combo boxes handles
	HWND hCBLighting, hCBTess, hCBShowNormals, hCBWireframe, hTBTessLevel;
	HWND hSTMSAA;
	HDC winDC;
	DEVMODE dmSettings;
	HANDLE AppMutex;
	HMENU menuHandle;
	HINSTANCE hInst;
	bool winActive;
	LPCWSTR szWinClassName;
	LPCWSTR szTitle;
	glCtrl ctrl;

	long int lastframeTime;
	float lastframeIval;
	void ErrorMessage(LPCWSTR text);
	ATOM WINAPI RegWindowClass();
	void WINAPI UnRegWindowClass();
	bool RecreateWindow();
	void SetToDesktopCenter();
	HICON loadIcon(INT id);
	HCURSOR loadCursor(INT id);
public:
	WinApp(INT wWidth, INT wHeight,LPCWSTR szAppTitle, LPCWSTR szClassName,HINSTANCE _hInst);
	bool InitMutex();
	LRESULT CALLBACK WndProcINT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	~WinApp(void);
	bool createWindow();
	void showWindow(int cmdShow);
	void mainLoop(MSG *msg);
	/*get functions */
	void get_AppName(LPWSTR out, int sz) {
		wcsncpy(out,szWinClassName,sz);
	}
	HWND getHandle() { return handle; }
		void resetTimer();
	void updateTimer();
	void refresh();
};

#endif
#endif

