#include "glCtrl.h"
#include "resource.h"
#include "controlDefs.h"
#include <ctime>
glCtrl::glCtrl() : clientWidth(0), clientHeight(0), GLinit(false), iCurrentFPS(0), iFPScount(0) {

}
int glCtrl::close()
{
	// close OpenGL Rendering context
	viewGL.closeContext(handle);
	return 0;
}
int glCtrl::destroy()
{
	viewGL.closeContext(handle);
	PostQuitMessage(0);       // exit the message loop
	return 0;
}
int glCtrl::mouseWheel(int state, int d, int x, int y) {
	modelGL.zoomCamera(d);
	return 0;
}
int glCtrl::create()
{
	return 0;
}
int glCtrl::initOpenGL(HDC *hdc,HWND *handle, HINSTANCE *hInst) {
	this->handle = *handle;
	// create a OpenGL rendering context
	if(!viewGL.initGLEW(hInst))
		return -1;
	if(!viewGL.createContext(handle,hdc, 32, 24, 8,4,3))
	{
		return -1;
	}
	curHostInfo = viewGL.getHostInfo();
	
	modelGL.setWinHandle(*handle); // for Error messaging
	modelGL.setDevContext(*hdc);
	// configure projection matrix and camera
	RECT rect;
	GetClientRect(*handle, &rect);	
	modelGL.setViewport(rect.right, rect.bottom);
	// initialize OpenGL states
	if(!modelGL.init()) {
		return -1;
	}
	GLinit = true;
	return 0;
}
int glCtrl::paint()
{
	if(GLinit) {
		BYTE Keys = 0x00;
		if(GetKeyState('W') & 0x80) Keys |= 0x01;
		if(GetKeyState('S') & 0x80) Keys |= 0x02;
		if(GetKeyState('A') & 0x80) Keys |= 0x04;
		if(GetKeyState('D') & 0x80) Keys |= 0x08;
		if(GetKeyState('R') & 0x80) Keys |= 0x10;
		if(GetKeyState('F') & 0x80) Keys |= 0x20;

		if(GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
		if(GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;
		modelGL.moveCameraByKB(Keys);
		long int currentClock = clock();
		long int iVal = currentClock - lastSecond;
		if((iVal) >= CLOCKS_PER_SEC)
		{
			lastSecond = currentClock;
			iFPScount = GLfloat(iCurrentFPS) / (GLfloat(iVal) / GLfloat(CLOCKS_PER_SEC));
			iCurrentFPS = 0;
		}
		modelGL.draw();
		iCurrentFPS++;
	}
	return 0;
}
int glCtrl::command(int id, int cmd, LPARAM msg)
{
	WCHAR *str;
	switch(id) {
	case ID_MENU_EXIT:
		if(MessageBox(handle,L"Уже уходите?",L"ICG GL Lab2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
			PostQuitMessage(EXIT_SUCCESS);
		}
		break;
	case ID_MENU_LOADDATA:
		// Add load func 
		modelGL.loadFileData();
		modelGL.initBuffers();
		modelGL.checkReadyToDraw();
		break;
	case ID_MENU_TECHDATA:
		str = new WCHAR[1024];
		swprintf(str,2048,L"Vendor: %S\nRenderer: %S\nOpenGL Version: %S\nGLSL version: %S\nSupported Extensions (num): %d",
			curHostInfo->vendor,curHostInfo->renderer,curHostInfo->GLversion,curHostInfo->GLSLversion,curHostInfo->numExts);
		MessageBox(handle,str,L"Technical Data",MB_OK | MB_ICONINFORMATION);
		delete[] str;
		break;
	case ID_MENU_ABOUT:
		
		break;
	case IDC_BUTTON_LIGHT:
		modelGL.changeLight();

		break;
	}
	return 0;
}
int glCtrl::wActivate(int aCmd,int minimized, HWND hWnd) {
	int globRes = -1;
	switch(aCmd) {
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		//resetTimer;
		globRes = 1;
		break;
	case WA_INACTIVE:
		globRes = 0;
		break;
	}
	return globRes;
}
int glCtrl::lButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL.setMousePosition(x, y);

	if(state == MK_LBUTTON)
	{
		modelGL.setMouseLeft(true);

	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
int glCtrl::lButtonUp(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL.setMousePosition(x, y);

	modelGL.setMouseLeft(false);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse down
///////////////////////////////////////////////////////////////////////////////
int glCtrl::rButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL.setMousePosition(x, y);

	if(state == MK_RBUTTON)
	{
		modelGL.setMouseRight(true);
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse up
///////////////////////////////////////////////////////////////////////////////
int glCtrl::rButtonUp(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL.setMousePosition(x, y);

	modelGL.setMouseRight(false);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEMOVE
///////////////////////////////////////////////////////////////////////////////
int glCtrl::mouseMove(WPARAM state, int x, int y)
{
	if(state == MK_LBUTTON)
	{
		modelGL.rotateCamera(x, y);
	}	
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_KEYDOWN
///////////////////////////////////////////////////////////////////////////////
int glCtrl::keyDown(int key, LPARAM lParam)
{
	switch(key) {
	case VK_ESCAPE:
		if(MessageBox(handle,L"Уже уходите?",L"ICG GL Lab2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
			PostQuitMessage(EXIT_SUCCESS);
		}
		break;
	case 0x4E: 
		// 'N' key virtual code http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
		modelGL.changeShowNormals();
		break;
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE notification
// Note that the input param, width and height is for client area only.
// It excludes non-client area.
///////////////////////////////////////////////////////////////////////////////
int glCtrl::size(int width, int height, WPARAM type)
{
	clientWidth = width;
	clientHeight = height;
	modelGL.setViewport(width,height);

	return 0;
}

glCtrl::~glCtrl(void)
{
}
