#pragma once
#ifndef GLVIEW_H
#define GLVIEW_H
#include <Windows.h>

class glView
{
	static bool setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits);
	static int findPixelFormat(HDC hdc, int colorbits, int depthBits, int stencilBits); // return best matched format ID
	bool setText2Dparams(); // set parameters for text output

	HDC hdc;                                        // handle to device context
	HGLRC hglrc, m_hrc;                                    // handle to OpenGL rendering context
public:
	bool createContext(HWND handle, int colorBits, int depthBits, int stencilBits);  // create OpenGL rendering context

	void closeContext(HWND handle);
	void swapBuffers();

	HDC getDC() const { return hdc; };
	HGLRC getRC() const { return m_hrc; };
	glView(void);
	~glView(void);
};
#endif

