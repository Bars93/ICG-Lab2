#include "GL/glew.h"
#include "GL/wglew.h"
#include "glView.h"
#pragma comment(lib,"glew32s.lib")

glView::glView(void):
	hdc(0), hglrc(0)
{
}


glView::~glView(void)
{
}
void glView::closeContext(HWND handle)
{
	if(!hdc || !hglrc)
		return;

	// delete DC and RC
	wglMakeCurrent(0, 0);
	wglDeleteContext(hglrc);
	ReleaseDC(handle, hdc);

	hdc = 0;
	hglrc = 0;
}



///////////////////////////////////////////////////////////////////////////////
// create OpenGL rendering context
///////////////////////////////////////////////////////////////////////////////
bool glView::createContext(HWND handle, int colorBits, int depthBits, int stencilBits)
{
	// retrieve a handle to a display device context
	hdc = GetDC(handle);

	// set pixel format
	if(!setPixelFormat(hdc, colorBits, depthBits, stencilBits))
	{
		MessageBox(0, L"Cannot set a suitable pixel format.", L"Error", MB_ICONEXCLAMATION | MB_OK);
		ReleaseDC(handle, hdc);                     // remove device context
		return false;
	}

	// create a new OpenGL rendering context
	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(handle,L"GLEW is not initialized!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};
	if(wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		m_hrc = wglCreateContextAttribsARB(hdc,0, attribs);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(hglrc);
		wglMakeCurrent(hdc, m_hrc);
	}
	else
	{       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		m_hrc = hglrc;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	WCHAR str[20];
	const GLubyte *glextstr = glGetString(GL_EXTENSIONS);
	wsprintf(str,L"OpenGL %d.%d",OpenGLVersion[0],OpenGLVersion[1]);
	MessageBox(handle,str,L"ICG GL Lab2", MB_OK | MB_ICONINFORMATION);
	MessageBoxA(handle,(char*)glextstr,"ICG GL Lab2",MB_OK | MB_ICONINFORMATION);
	if (!m_hrc) return false;
	return true;
}



///////////////////////////////////////////////////////////////////////////////
// choose pixel format
// By default, pdf.dwFlags is set PFD_DRAW_TO_WINDOW, PFD_DOUBLEBUFFER and PFD_SUPPORT_OPENGL.
///////////////////////////////////////////////////////////////////////////////
bool glView::setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits)
{
	PIXELFORMATDESCRIPTOR pfd, *ppfd = &pfd;
	ZeroMemory(&pfd,sizeof(pfd));
	int pixelFormat;
	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	ppfd->nVersion = 1; 
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	ppfd->dwLayerMask = PFD_MAIN_PLANE; 
	ppfd->iPixelType = PFD_TYPE_RGBA; 
	ppfd->cColorBits = 32; 
	ppfd->cDepthBits = 32; 

	// find out the best matched pixel format
	/* int pixelFormat = findPixelFormat(hdc, colorBits, depthBits, stencilBits);
	if(pixelFormat == 0)
	return false;
	*/
	// set members of PIXELFORMATDESCRIPTOR with given mode ID
	//    DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd);
	if ( (pixelFormat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
	{ 
		MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK); 
		return FALSE; 
	} 
	// set the fixel format
	if(!SetPixelFormat(hdc, pixelFormat, &pfd))
		return false;

	return true;
}



///////////////////////////////////////////////////////////////////////////////
// find the best pixel format
///////////////////////////////////////////////////////////////////////////////
int glView::findPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits)
{
	int currMode;                               // pixel format mode ID
	int bestMode = 0;                           // return value, best pixel format
	int currScore = 0;                          // points of current mode
	int bestScore = 0;                          // points of best candidate
	PIXELFORMATDESCRIPTOR pfd;

	// search the available formats for the best mode
	bestMode = 0;
	bestScore = 0;
	for(currMode = 1; DescribePixelFormat(hdc, currMode, sizeof(pfd), &pfd) > 0; ++currMode)
	{
		// ignore if cannot support opengl
		if(!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
			continue;

		// ignore if cannot render into a window
		if(!(pfd.dwFlags & PFD_DRAW_TO_WINDOW))
			continue;

		// ignore if cannot support rgba mode
		if((pfd.iPixelType != PFD_TYPE_RGBA) /*|| (pfd.dwFlags & PFD_NEED_PALETTE)*/)
			continue;

		// ignore if not double buffer
		//if(!(pfd.dwFlags & PFD_DOUBLEBUFFER))
		//  continue;

		// try to find best candidate
		currScore = 0;

		// colour bits
		if(pfd.cColorBits >= colorBits) ++currScore;
		if(pfd.cColorBits == colorBits) ++currScore;

		// depth bits
		if(pfd.cDepthBits >= depthBits) ++currScore;
		if(pfd.cDepthBits == depthBits) ++currScore;

		// stencil bits
		// if(pfd.cStencilBits >= stencilBits) ++currScore;
		if(pfd.cStencilBits == stencilBits) ++currScore;

		// alpha bits
		if(pfd.cAlphaBits > 0) ++currScore;

		// check if it is best mode so far
		if(currScore > bestScore)
		{
			bestScore = currScore;
			bestMode = currMode;
		}
	}

	return bestMode;
}



///////////////////////////////////////////////////////////////////////////////
void glView::swapBuffers()
{
	::SwapBuffers(hdc);
}