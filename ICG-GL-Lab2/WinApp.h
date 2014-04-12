#pragma warning(disable : 4996)
#ifdef _WIN32 // Windows only
#ifndef WINAPP_H
#define WINAPP_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h> 
#include <wchar.h> // for UNICODE characters functions
#include "WinCtrl.h"
class WinApp
{
	int width, height;
	int x,y;
	HWND handle, parentHandle;
	HMENU menuHandle;
	HINSTANCE hInst;
	bool fullscreen;
	LPCWSTR szWinClassName;
	LPCWSTR szTitle;
	WinCtrl *lpCtrl;
	void ErrorMessage(LPCWSTR text);
	ATOM WINAPI RegWindow();
	HICON loadIcon(INT id);
	HCURSOR loadCursor(INT id);
public:
	WinApp(int _w, int _h,WinCtrl *ctrl,LPCWSTR szAppTitle, LPCWSTR szClassName,bool fs, HWND hParent,HINSTANCE _hInst);
	~WinApp(void);
	bool createWindow(DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, DWORD dwExStyle = WS_EX_APPWINDOW);
	void showWindow(int cmdShow);
	/*get functions */
	void get_AppName(LPWSTR out, int sz) {
		wcsncpy(out,szWinClassName,sz);
	}
	HWND getHandle() { return handle; }
};

#endif
#endif

