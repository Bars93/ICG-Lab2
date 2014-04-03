#pragma once
#ifndef GLVIEW_H
#define GLVIEW_H
#include "glHeaders.h"

#define OGL_V10 MAKEWORD(1,0)
#define OGL_V11 MAKEWORD(1,1)
#define OGL_V12 MAKEWORD(1,2)
#define OGL_V13 MAKEWORD(1,3)
#define OGL_V14 MAKEWORD(1,4)
#define OGL_V15 MAKEWORD(1,5)
#define OGL_V20 MAKEWORD(2,0)
#define OGL_V21 MAKEWORD(2,1)
#define OGL_V30 MAKEWORD(3,0)
#define OGL_V31 MAKEWORD(3,1)
#define OGL_V32 MAKEWORD(3,2)
#define OGL_V33 MAKEWORD(3,3)
#define OGL_V40 MAKEWORD(4,0)
#define OGL_V41 MAKEWORD(4,1)
#define OGL_V42 MAKEWORD(4,2)
#define OGL_V43 MAKEWORD(4,3)
#define OGL_V44 MAKEWORD(4,4)

class glView
{
	static bool setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits);
	static int findPixelFormat(HDC hdc, int colorbits, int depthBits, int stencilBits); // return best matched format ID
	bool setText2Dparams(); // set parameters for text output

	HDC hdc;                                        // handle to device context
	HGLRC hglrc, m_hrc;                                    // handle to OpenGL rendering context, m_hrc for OpenGL 3.0+ context
public:
	bool createContext(HWND handle, int colorBits, int depthBits, int stencilBits, DWORD oglVersion);  // create OpenGL rendering context
	void closeContext(HWND handle);
	void swapBuffers();

	HDC getDC() const { return hdc; };
	HGLRC getRC() const { return m_hrc; };
	glView(void);
	~glView(void);
};
#endif

