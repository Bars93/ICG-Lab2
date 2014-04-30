#pragma warning(disable : 4996)
#include "WinApp.h"
#include "WinProc.h"
#include "resource.h"
HICON WinApp::loadIcon(int id)
{
	return (HICON)::LoadImage(hInst, MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
}
HICON WinApp::loadCursor(int id)
{
	return (HCURSOR)::LoadImage(hInst, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
}

void WinApp::ErrorMessage(LPCWSTR text) {
	MessageBox(handle,text,szWinClassName, MB_OK | MB_ICONERROR);
}
WinApp::WinApp(int _w, int _h,WinCtrl *ctrl,LPCWSTR szAppTitle,LPCWSTR szClassName,bool fs, HWND hParent,HINSTANCE _hInst) : 
	handle(0),
	lpCtrl(ctrl),
	x(CW_USEDEFAULT),y(CW_USEDEFAULT),
	width(_w),height(_h),
	szWinClassName(szClassName),
	menuHandle(NULL),
	szTitle(szAppTitle),
	parentHandle(hParent),
	fullscreen(fs),
	hInst(_hInst)
{
}
ATOM WINAPI WinApp::RegWindow() {
	ATOM globRes;
	WNDCLASSEX   WCE; 
	ZeroMemory(&WCE,sizeof(WCE));
	// set window class parameters
	WCE.cbSize = sizeof(WNDCLASSEX);
	WCE.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; 
	WCE.lpfnWndProc   = (WNDPROC) mainWndProc; 
	WCE.hInstance     = hInst; 
	WCE.hIcon         = loadIcon(IDI_OPENGL_ICON);
	WCE.hIconSm       = loadIcon(IDI_OPENGL_ICON);
	WCE.hCursor       = LoadCursor (NULL,IDC_ARROW); 
	WCE.lpszClassName = szWinClassName;
	WCE.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	globRes = RegisterClassEx(&WCE);
	if(!globRes) {
		ErrorMessage(L"Error register window class!");
	}
	return globRes;
}
bool WinApp::createWindow(DWORD dwStyle, DWORD dwExStyle) {
	bool globRes = true;
	if(!RegWindow()) {
		globRes = false;
	}
	if(globRes) {
		handle = CreateWindowEx(dwExStyle,szWinClassName,szTitle,dwStyle,x,y,width,height,HWND_DESKTOP,menuHandle,hInst,(LPVOID)lpCtrl);
		if(!handle) {
			globRes = false;
			ErrorMessage(L"Error window creation");
		}
	}
	else {
		ErrorMessage(L"Error Window registration!");
	}
	return globRes;
}
void WinApp::showWindow(int cmdShow) {
	ShowWindow(handle, cmdShow);
	UpdateWindow(handle);
}
WinApp::~WinApp(void)
{
	// specific "desctructions"
	DestroyWindow(handle);
	UnregisterClass(szWinClassName,hInst);
}
