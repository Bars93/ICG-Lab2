#include "glView.h"
LRESULT CALLBACK WndProcFake(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
taghostInfo::taghostInfo() :
	vendor(NULL),
	GLversion(NULL),
	GLSLversion(NULL),
	renderer(NULL),
	numExts(GL_ZERO)
{
	exts.clear();
	GLversioni.major = GLversioni.minor = GL_ZERO;
}
glView::glView(void):
	hdc(0), hglrc(0),
	glewInitialized(false)
{
	ZeroMemory(&glHostInfo,sizeof(hostInfo));
}

glView::~glView(void)
{
}
bool glView::initGLEW(HINSTANCE *hInst) {
	LPCWSTR szFakeClass = L"oglFakeWindow";
	if(glewInitialized)
		return true;
	WNDCLASSEX fakeWCE;
	ZeroMemory(&fakeWCE,sizeof(WNDCLASSEX));
	fakeWCE.cbSize = sizeof(WNDCLASSEX);
	fakeWCE.style =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	fakeWCE.lpfnWndProc = (WNDPROC) WndProcFake;
	fakeWCE.cbClsExtra = 0; fakeWCE.cbWndExtra = 0;
	fakeWCE.hInstance = *hInst;
	fakeWCE.hIcon = LoadIcon(*hInst, MAKEINTRESOURCE(IDI_APPLICATION));
	fakeWCE.hIconSm = LoadIcon(*hInst, MAKEINTRESOURCE(IDI_APPLICATION));
	fakeWCE.hCursor = LoadCursor(NULL, IDC_ARROW);
	fakeWCE.hbrBackground = (HBRUSH)(COLOR_MENUBAR+1);
	fakeWCE.lpszMenuName = NULL;
	fakeWCE.lpszClassName = szFakeClass;

	RegisterClassEx(&fakeWCE);

	HWND hWndFake = CreateWindow(szFakeClass, L"FAKE", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, *hInst, NULL);

	HDC fakeDC = GetDC(hWndFake);

	// First, choose false pixel format
	if(!setPixelFormat(fakeDC,32,24,8))
		return false;

	// Create the false, old style context (OpenGL 2.1 and before)
	HGLRC hRCFake = wglCreateContext(fakeDC);
	wglMakeCurrent(fakeDC, hRCFake);

	bool bResult = true;

	if(!glewInitialized)
	{
		glewExperimental = true;
		if(glewInit() != GLEW_OK)
		{
			MessageBox(hWndFake, L"Couldn't initialize GLEW!", L"Fatal Error", MB_ICONERROR);
			bResult = false;
		}
		glewInitialized = true;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCFake);
	ReleaseDC(hWndFake,fakeDC);
	DestroyWindow(hWndFake);
	UnregisterClass(szFakeClass,*hInst);
	return bResult;
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
bool glView::createContext(HWND *handle, HDC *AppWinDC, int colorBits, int depthBits, int stencilBits, GLuint oglMajorVersion, GLuint oglMinorVersion)
{
	// retrieve a handle to a display device context
	hdc = *AppWinDC;
	AppWinHandle = *handle;
	// set pixel format
	if(!setPixelFormat(hdc, colorBits, depthBits, stencilBits))
	{
		MessageBox(0, L"Cannot set a suitable pixel format.", L"GL View Class", MB_ICONEXCLAMATION | MB_OK);
		ReleaseDC(AppWinHandle, hdc);                     // remove device context
		return false;
	}

	// create a new OpenGL rendering context
	if(oglMajorVersion == 1  && oglMinorVersion <= 5 ||
		oglMajorVersion == 2 && (oglMinorVersion == 0 || oglMinorVersion == 1)) {
			hglrc = wglCreateContext(hdc);
			if(hglrc == NULL) {
				MessageBox(AppWinHandle,L"wglCreateContext error! GL context creation failed!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return false;
			}
			if( (wglMakeCurrent(hdc, hglrc)) == FALSE) { // Create OpenGL 2.1 (GLSL 1.30) or earlier context
				MessageBox(AppWinHandle,L"wglMakeCurrent error! GL context creation failed!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return false;
			}
			m_hrc = hglrc;
	}
	else if(oglMajorVersion == 3  && oglMinorVersion <= 3 || 
		oglMajorVersion == 4 &&  oglMinorVersion <= 4) {
			static const GLint PFAttribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				0
			};
			static const GLint ContextAttribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, oglMajorVersion,
				WGL_CONTEXT_MINOR_VERSION_ARB, oglMinorVersion,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};
			/* Create OpenGL 3.0+ (GLSL 1.50+) context, using GLEW, future realise */
			hglrc = wglCreateContext(hdc);
			if(hglrc == NULL) {
				MessageBox(AppWinHandle,L"wglCreateContext error! GL context creation failed!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return false;
			}
			if( (wglMakeCurrent(hdc, hglrc)) == FALSE) { // Create OpenGL 2.1 (GLSL 1.30) or earlier context
				MessageBox(AppWinHandle,L"wglMakeCurrent error! GL context creation failed!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return false;
			}

			if(WGLEW_ARB_create_context && WGLEW_ARB_pixel_format)
			{
				int iPixelFormat, iNumFormats;
				wglChoosePixelFormatARB(hdc, PFAttribs, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);
				PIXELFORMATDESCRIPTOR pfd;
				ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));
				// PFD seems to be only redundant parameter now
				if(!SetPixelFormat(hdc, iPixelFormat, &pfd))
					return false;
				m_hrc = wglCreateContextAttribsARB(hdc,0, ContextAttribs);
				if(m_hrc == NULL) {
					MessageBox(AppWinHandle,L"Error creating context with current attributes! Return to old style context!",L"ICG GL Lab2", MB_OK | MB_ICONEXCLAMATION);
					m_hrc = hglrc;
					if(wglMakeCurrent(hdc, m_hrc) == GL_FALSE) {
						MessageBox(AppWinHandle,L"Error choosing context!",L"ICG GL Lab2", MB_OK | MB_ICONEXCLAMATION);
						return false;
					}
				}
				else {
					wglMakeCurrent(NULL,NULL);
					wglDeleteContext(hglrc);
					if(wglMakeCurrent(hdc, m_hrc) == GL_FALSE) {
						MessageBox(AppWinHandle,L"Error choosing context!",L"ICG GL Lab2", MB_OK | MB_ICONEXCLAMATION);
						return false;
					}

				}
			}
			else
			{       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
				m_hrc = hglrc;
				if(wglMakeCurrent(hdc, m_hrc) == GL_FALSE) {
					MessageBox(AppWinHandle,L"Error choosing context!",L"ICG GL Lab2", MB_OK | MB_ICONEXCLAMATION);
					return false;
				}
			}
			GLuint error = OGLCheckError(errorString,L"createContext",1024);
			if(error > 0) {
				MessageBox(AppWinHandle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return 0;
			}
			while(error > 0)
				error = glGetError();
			//Checking GL version
			if(wglSwapIntervalEXT != NULL) {
				wglSwapIntervalEXT(GL_ZERO);
			}
			if(!GLEW_VERSION_4_3)
			{
				MessageBox(AppWinHandle,L"OpenGL 4.3 not supported!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return false;
			}
			getGLHostInfo();
	}
	else {
		MessageBox(AppWinHandle,L"No currently availible OpenGL version",L"ICG GL Lab2", MB_OK | MB_ICONINFORMATION);
	}
	if (!m_hrc) return false;
	return true;
}
#define GLubyteToCCHAR(str) ((const char *)str)
void glView::getGLHostInfo() {
	glGetIntegerv(GL_MAJOR_VERSION, &glHostInfo.GLversioni.major);
	glGetIntegerv(GL_MINOR_VERSION, &glHostInfo.GLversioni.minor);
	glHostInfo.GLversion = const_cast<GLubyte*>(glGetString(GL_VERSION));
	glHostInfo.renderer = const_cast<GLubyte*>(glGetString(GL_RENDERER));
	glHostInfo.GLSLversion = const_cast<GLubyte*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	glHostInfo.vendor = const_cast<GLubyte*>(glGetString(GL_VENDOR));
	glGetIntegerv(GL_NUM_EXTENSIONS,&glHostInfo.numExts);
	int i = 0;
	while(i < glHostInfo.numExts) {
		const GLubyte* gotExt = glGetStringi(GL_EXTENSIONS,i);
		string pushExt((char*)const_cast<GLubyte*>(gotExt));
		glHostInfo.exts.push_back(pushExt);
		i++;
	}
}
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
	ppfd->cColorBits = colorBits; 
	ppfd->cDepthBits = depthBits; 
	ppfd->cStencilBits = stencilBits;
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



