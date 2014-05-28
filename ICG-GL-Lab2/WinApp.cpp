#pragma warning(disable : 4996)
#include "WinApp.h"
#include "WinProc.h"
#include "resource.h"
#include "controlDefs.h"
#include <ctime>
const DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
const DWORD dwStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
const DWORD dwFSStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
const DWORD dwFSStyleEx = WS_EX_APPWINDOW;
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
WinApp::WinApp(INT wWidth, INT wHeight,LPCWSTR szAppTitle,LPCWSTR szClassName, HINSTANCE _hInst) : 
	handle(0),
	x(CW_USEDEFAULT),y(CW_USEDEFAULT),
	width(wWidth),height(wHeight),
	szWinClassName(szClassName),
	menuHandle(NULL),
	szTitle(szAppTitle),
	hInst(_hInst),
	winDC(NULL),
	winActive(true),
	fullscreen(false)
{
}
void WinApp::refresh() {
	SendMessage(handle,WM_PAINT,0,0);	
}
ATOM WINAPI WinApp::RegWindowClass() {
	ATOM globRes;
	WNDCLASSEX   WCE; 
	ZeroMemory(&WCE,sizeof(WCE));
	// set window class parameters
	WCE.cbSize = sizeof(WNDCLASSEX);
	WCE.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; 
	WCE.lpfnWndProc   = (WNDPROC) WndProcEXT; 
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
void WinApp::UnRegWindowClass() {
	if(handle)
		DestroyWindow(handle);
	handle = NULL;
	UnregisterClass(szWinClassName,hInst);
}
bool WinApp::createWindow() {
	bool globRes = true;
	if(!RegWindowClass()) {
		globRes = false;
	}
	if(globRes) {
		INITCOMMONCONTROLSEX InitCtrls; //для контролов
		InitCtrls.dwSize = sizeof(InitCtrls); //для контролов
		InitCtrls.dwICC = ICC_WIN95_CLASSES; //для контролов
		InitCommonControlsEx(&InitCtrls); //для контролов
		handle = CreateWindowEx(dwStyleEx,szWinClassName,szTitle,dwStyle,x,y,width,height,NULL,(HMENU)menuHandle,hInst,(LPVOID)NULL);
		if(!handle) {
			globRes = false;
			ErrorMessage(L"Error window creation");
		}
		if(globRes) {
			winDC = GetDC(handle);
			if(ctrl.initOpenGL(&winDC,&handle,&hInst) < 0) {
				globRes = false;
			}
			//hBtnState = CreateWindowEx(NULL,L"BUTTON",L"Освещение",WS_CHILD | WS_VISIBLE, width - 170,10,140,25,handle,(HMENU)IDC_BUTTON_LIGHT,hInst,(LPVOID)0);
			hCBLighting = CreateWindowEx(NULL,WC_BUTTON,L"Освещение", BS_CHECKBOX | WS_CHILD | WS_VISIBLE,width - 195,10,190,15,handle,(HMENU)IDC_CB_LIGHTING,hInst,(LPVOID)0);
			hCBShowNormals = CreateWindowEx(NULL,WC_BUTTON,L"Отобразить нормали", BS_CHECKBOX | WS_CHILD | WS_VISIBLE,width - 195,30,190,15,handle,(HMENU)IDC_CB_SHOWNORMALS,hInst,(LPVOID)0);
			hCBWireframe  = CreateWindowEx(NULL,WC_BUTTON,L"Wireframe", BS_CHECKBOX | WS_CHILD | WS_VISIBLE,width - 195,50,190,15,handle,(HMENU)IDC_CB_WIREFRAME,hInst,(LPVOID)0);
			hCBSmoothNormals  = CreateWindowEx(NULL,WC_BUTTON,L"Сглаживание нормалей", BS_CHECKBOX | WS_CHILD | WS_VISIBLE | BS_MULTILINE,width - 195,70,190,30,handle,(HMENU)IDC_CB_SMOOTHNORMALS,hInst,(LPVOID)0);
			hCBTexturing = CreateWindowEx(NULL,WC_BUTTON,L"Текстурирование", BS_CHECKBOX | WS_CHILD | WS_VISIBLE,width - 195,110,190,15,handle,(HMENU)IDC_CB_TEXTURE,hInst,(LPVOID)0);
			if(!hCBLighting || !hCBShowNormals || !hCBWireframe || !hCBSmoothNormals || !hCBTexturing) {
				globRes = false;
				ErrorMessage(L"Button creation failed");
			}
		}
	}
	else {
		ErrorMessage(L"Error Window registration!");
	}
	return globRes;
}
void WinApp::resetTimer() {
	lastframeTime = std::clock();
	lastframeIval = 0.0f;
}
void WinApp::updateTimer() {
	long int CurrentTime = std::clock();
	lastframeIval = float(CurrentTime-lastframeTime)/float(CLOCKS_PER_SEC);
	lastframeTime = CurrentTime;
}
void WinApp::setToDesktopCenter() {
	RECT descRect, winRect;
	GetClientRect(HWND_DESKTOP,&descRect);
	

}
void WinApp::mainLoop(MSG *msg) {
	static WCHAR newSzTitle[256];
	resetTimer();
	while(true) {
		if(PeekMessage(msg,NULL,0,0,PM_REMOVE)) {
			if(msg->message == WM_QUIT) break;
			else {
				TranslateMessage(msg);
				DispatchMessage(msg);
			}
		}
		if(winActive) {
			//timerUpdate
			updateTimer();
			ctrl.paint();
			swprintf(newSzTitle,256,L"ICG GL Lab-2. FPS - %.2f.",ctrl.getFPS(),this->lastframeIval);
			SetWindowText(handle,newSzTitle);
		}
		else {
			Sleep(200);
		}
	}

}
LRESULT CALLBACK WinApp::WndProcINT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT returnValue = 0;        // return value
	// route messages to the associated controller
	switch(msg)
	{
	case WM_CREATE:
		srand((UINT)time(NULL));
		returnValue = ctrl.create();
		break;

	case WM_SIZE:
		this->width = LOWORD(lParam);
		this->height = HIWORD(lParam);
		MoveWindow(hCBLighting,width - 195,10,190,15,true);
		MoveWindow(hCBShowNormals,width - 195,30,190,15,true);
		MoveWindow(hCBWireframe,width - 195,50,190,15,true);
		MoveWindow(hCBSmoothNormals,width - 195,70,190,30,true);
		MoveWindow(hCBTexturing,width - 195,110,190,15,true);
		returnValue = ctrl.size(LOWORD(lParam), HIWORD(lParam), (int)wParam);    // width, height, type
		break;

	case WM_MOUSEWHEEL: 
		{
			returnValue = ctrl.mouseWheel(GET_KEYSTATE_WPARAM(wParam), // Keystate
				GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, // delta of moving
				GET_X_LPARAM(lParam), // x coordinate
				GET_Y_LPARAM(lParam)); // y coordinate
			break;
		}
	case WM_PAINT:
		static PAINTSTRUCT ps;
		BeginPaint(hwnd,&ps);
		EndPaint(hwnd,&ps);
		//InvalidateRect( hwnd, NULL, FALSE );
		returnValue = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	case WM_NCPAINT:  /* 0x0085 */
		/* Need to update the border of this window */
		returnValue = DefWindowProc( hwnd, msg, wParam, lParam );
		/* Pass it on to "DefWindowProc" to repaint a standard border */
		break;
	case WM_COMMAND:
		returnValue = ctrl.command(hwnd,LOWORD(wParam), HIWORD(wParam), lParam);   // id, code, msg
		break;

	case WM_MOUSEMOVE:
		returnValue = ctrl.mouseMove(wParam, LOWORD(lParam), HIWORD(lParam));  // state, x, y
		break;

	case WM_CLOSE:
		returnValue = ctrl.close();
		UnRegWindowClass();
		break;

	case WM_DESTROY:
		returnValue = ctrl.destroy();
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		returnValue = ctrl.keyDown((int)wParam, lParam);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		returnValue = ctrl.keyUp((int)wParam, lParam);                         // keyCode, keyDetail
		break;

	case WM_LBUTTONDOWN:
		returnValue = ctrl.lButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_LBUTTONUP:
		returnValue = ctrl.lButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_RBUTTONDOWN:
		returnValue = ctrl.rButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_RBUTTONUP:
		returnValue = ctrl.rButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_MBUTTONDOWN:
		returnValue = ctrl.mButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_MBUTTONUP:
		returnValue = ctrl.mButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_HSCROLL:
		returnValue = ctrl.hScroll(wParam, lParam);
		break;

	case WM_VSCROLL:
		returnValue = ctrl.vScroll(wParam, lParam);
		break;

	case WM_TIMER:
		returnValue = ctrl.timer(LOWORD(wParam), HIWORD(wParam));
		break;

	case WM_NOTIFY:
		returnValue = ctrl.notify((int)wParam, lParam);                        // controllerID, lParam
		break;

	case WM_CONTEXTMENU:
		returnValue = ctrl.contextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));    // handle, x, y (from screen coords)
		break;
	case WM_ERASEBKGND:
		returnValue = ctrl.eraseBkgnd((HDC)wParam);                            // handle to device context
		break;
	case WM_SYSCOMMAND:
		//returnValue = ctrl.sysCommand(wParam, lParam);
		returnValue = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	case WM_ACTIVATE:
		returnValue = ctrl.wActivate(LOWORD(wParam),HIWORD(wParam),(HWND)lParam);
		if(returnValue == 1) {
			this->winActive = true;
			resetTimer();
		}
		else
			this->winActive = false;
		break;
		// Disable Alt-F4 and screensavers
	default:
		returnValue = DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return returnValue;
}
bool WinApp::InitMutex() {
	AppMutex = CreateMutex(NULL, 1, szWinClassName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, L"This application already runs!", L"Multiple Instances Found.", MB_ICONINFORMATION | MB_OK);
		return 0;
	}
	return 1;
}
void WinApp::showWindow(int cmdShow) {
	if(handle) {
		ShowWindow(handle, cmdShow);
		UpdateWindow(handle);
		//RedrawWindow(handle,NULL,NULL,RDW_NOERASE | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
		SetForegroundWindow(handle);
		SetFocus(handle);
	}
}
WinApp::~WinApp(void)
{
	// specific "desctructions"
	DestroyWindow(handle);
	UnregisterClass(szWinClassName,hInst);
	ReleaseMutex(AppMutex);
}
