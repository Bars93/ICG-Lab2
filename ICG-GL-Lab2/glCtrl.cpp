#include "glCtrl.h"
#include <process.h>

glCtrl::glCtrl(glView* view,glModel* model) : modelGL(model), viewGL(view),
	threadHandle(0), threadId(0),
	loopFlag(false), resizeFlag(false),
	clientWidth(0), clientHeight(0)
{
}
int glCtrl::close()
{
	loopFlag = false;
	::WaitForSingleObject(threadHandle, INFINITE);  // wait for rendering thread is terminated

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
int glCtrl::create()
{
	// create a OpenGL rendering context
	if(!viewGL->createContext(handle, 32, 24, 0))
	{
		//Win::log(L"[ERROR] Failed to create OpenGL rendering context from glCtrl::create().");
		return -1;
	}

	// create a thread for OpenGL rendering
	// The params of _beginthreadex() are security, stackSize, functionPtr, argPtr, initFlag, threadId.
	threadHandle = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall *)(void *))threadFunction, this, 0, &threadId);
	if(threadHandle)
	{
		loopFlag = true;
		//Win::log(L"Created a rendering thread for OpenGL.");
	}
	else
	{
		//Win::log(L"[ERROR] Failed to create rendering thread from glCtrl::create().");
	}

	return 0;
}
int glCtrl::paint()
{
	return 0;
}
int glCtrl::command(int id, int cmd, LPARAM msg)
{
	return 0;
}
void glCtrl::threadFunction(void* param)
{
	((glCtrl*)param)->runThread();
}
void glCtrl::runThread()
{
	// set the current RC in this thread
	wglMakeCurrent(viewGL->getDC(), viewGL->getRC());

	// initialize OpenGL states
	modelGL->init();

	// cofigure projection matrix
	RECT rect;
	::GetClientRect(handle, &rect);
	modelGL->setViewport(rect.right, rect.bottom);

	// rendering loop
	while(loopFlag)
	{
		::Sleep(10);                    // yield to other processes or threads

		if(resizeFlag)
		{
			modelGL->setViewport(clientWidth, clientHeight);
			resizeFlag = false;
		}

		modelGL->draw();
		viewGL->swapBuffers();
	}

	// terminate rendering thread
	::wglMakeCurrent(0, 0);             // unset RC
	::CloseHandle(threadHandle);
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
    /*if(state == MK_LBUTTON)
    {
        modelGL->rotateCamera(x, y);
    }
    if(state == MK_RBUTTON)
    {
        modelGL->zoomCamera(y);
    }
*/
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
    resizeFlag = true;
    clientWidth = width;
    clientHeight = height;

    return 0;
}


glCtrl::~glCtrl(void)
{
}
