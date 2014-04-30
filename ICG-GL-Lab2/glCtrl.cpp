#include "glCtrl.h"
#include "resource.h"
#include "controlDefs.h"
glCtrl::glCtrl(glView* view,glModel* model) : modelGL(model), viewGL(view),
	threadHandle(0), threadId(0),
	clientWidth(0), clientHeight(0)
{
}
int glCtrl::close()
{
	//::WaitForSingleObject(threadHandle, INFINITE);  // wait for rendering thread is terminated

	// close OpenGL Rendering context
	viewGL->closeContext(handle);

	::DestroyWindow(handle);
	return 0;
}
int glCtrl::destroy()
{
	::PostQuitMessage(0);       // exit the message loop
	return 0;
}
int glCtrl::mouseWheel(int state, int d, int x, int y) {
	modelGL->zoomCamera(d);
	return 0;
}
int glCtrl::create()
{
	// create a OpenGL rendering context
	if(!viewGL->createContext(handle, 32, 24, 0, OGL_V21))
	{
		//Win::log(L"[ERROR] Failed to create OpenGL rendering context from glCtrl::create().");
		return -1;
	}
	modelGL->setWinHandle(handle); // for Error messaging

	// create a thread for OpenGL rendering
	// The params of _beginthreadex() are security, stackSize, functionPtr, argPtr, initFlag, threadId.
	//threadHandle = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall *)(void *))threadFunction, this, 0, &threadId);
	//if(threadHandle)
	//{
	//	loopFlag = true;
	//	//Win::log(L"Created a rendering thread for OpenGL.");
	//}
	//else
	//{
	//	//Win::log(L"[ERROR] Failed to create rendering thread from glCtrl::create().");
	//}
	//loopFlag = true;
	//runThread();

	// initialize OpenGL states
	if(!modelGL->init()) {
		return -1;
	}

	// configure projection matrix and camera
	RECT rect;
	::GetClientRect(handle, &rect);
	modelGL->setViewport(rect.right, rect.bottom);
	// rendering loop

	modelGL->draw();
	viewGL->swapBuffers();
	InvalidateRect(handle,NULL,FALSE);
	return 0;
}
int glCtrl::paint()
{
	PAINTSTRUCT ps;

	HDC hDC = BeginPaint(handle, &ps);
	ValidateRect(handle,FALSE);
	BYTE Keys = 0x00;
	if(GetKeyState('W') & 0x80) Keys |= 0x01;
	if(GetKeyState('S') & 0x80) Keys |= 0x02;
	if(GetKeyState('A') & 0x80) Keys |= 0x04;
	if(GetKeyState('D') & 0x80) Keys |= 0x08;
	if(GetKeyState('R') & 0x80) Keys |= 0x10;
	if(GetKeyState('F') & 0x80) Keys |= 0x20;

	if(GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
	if(GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;
	modelGL->moveCameraByKB(Keys);
	modelGL->draw();
	viewGL->swapBuffers();
	EndPaint(handle,&ps);
	return 0;
}
int glCtrl::command(int id, int cmd, LPARAM msg)
{
	switch(id) {
	case ID_MENU_EXIT:
		if(MessageBox(handle,L"Уже уходите?",L"ICG GL Lab2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
			PostQuitMessage(EXIT_SUCCESS);
		}
		break;
	case ID_MENU_LOADDATA:
		// Add load func call
		break;
	case ID_MENU_TECHDATA:

		break;
	case ID_MENU_ABOUT:

		break;
	case IDC_BUTTON_LIGHT:
		modelGL->changeLight();
		break;
	}
	return 0;
}
int glCtrl::lButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL->setMousePosition(x, y);

	if(state == MK_LBUTTON)
	{
		modelGL->setMouseLeft(true);
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
int glCtrl::lButtonUp(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL->setMousePosition(x, y);

	modelGL->setMouseLeft(false);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse down
///////////////////////////////////////////////////////////////////////////////
int glCtrl::rButtonDown(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL->setMousePosition(x, y);

	if(state == MK_RBUTTON)
	{
		modelGL->setMouseRight(true);
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse up
///////////////////////////////////////////////////////////////////////////////
int glCtrl::rButtonUp(WPARAM state, int x, int y)
{
	// update mouse position
	modelGL->setMousePosition(x, y);

	modelGL->setMouseRight(false);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEMOVE
///////////////////////////////////////////////////////////////////////////////
int glCtrl::mouseMove(WPARAM state, int x, int y)
{
	if(state == MK_LBUTTON)
	{
		modelGL->rotateCamera(x, y);

	}	
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_KEYDOWN
///////////////////////////////////////////////////////////////////////////////
int glCtrl::keyDown(int key, LPARAM lParam)
{
	if(key == VK_ESCAPE)
	{
		::PostMessage(handle, WM_CLOSE, 0, 0);
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
	modelGL->setViewport(width,height);
	return 0;
}


glCtrl::~glCtrl(void)
{
}
