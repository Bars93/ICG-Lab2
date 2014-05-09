#pragma once
#ifndef GLVIEW_H
#define GLVIEW_H
#include "glHeaders.h"
#include <vector>
#include <string>
using std::string;
using std::vector;
typedef struct taghostInfo {
	struct {
		int major;
		int minor;
	} GLversioni;
	GLubyte *vendor;
	GLubyte *renderer;
	GLubyte *GLversion;
	GLubyte *GLSLversion;
	vector<string> exts;
	int numExts;
	taghostInfo();
} hostInfo;
class glView
{
	
	WCHAR errorString[1024];
	bool glewInitialized;
	HWND AppWinHandle;
	hostInfo glHostInfo;
	HDC hdc;                                        // handle to device context
	HGLRC hglrc, m_hrc;                                    // handle to OpenGL rendering context, m_hrc for OpenGL 3.0+ context
	bool setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits);
	void getGLHostInfo();
public:
	bool initGLEW(HINSTANCE *hInst);
	bool createContext(HWND *handle, HDC *AppWinDC, int colorBits, int depthBits, int stencilBits, GLuint oglMajorVersion, GLuint oglMinorVersion);  // create OpenGL rendering context
	bool UnsetWindowContext();
	bool ReInitWindowContext(HWND *handle);
	void closeContext(HWND handle);
	void swapBuffers();
	hostInfo *getHostInfo() { return &(this->glHostInfo); };
	HDC getDC() const { return hdc; };
	HGLRC getRC() const { return m_hrc; };
	glView(void);
	~glView(void);
};
#endif

