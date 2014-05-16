#ifndef GLHEADERS_H
#define GLHEADERS_H
#define GL_ADVANCED
#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#if defined(_WIN32)
#include <Windows.h>
#include <GL/wglew.h>
#include <GL/wglext.h>
#else
#include <GL/glxew.h>
#endif
#include <GL/GL.h>
#include <GL/GLU.h>
#if defined(GL_ADVANCED)
#include <GL/glext.h>
#include <GL/glcorearb.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include <string>
inline bool operator==(const std::string &a,const std::string &b) {
	if(a.length() == b.length() && !strcmp(a.c_str(),b.c_str())) {
		return true;
	}
	else
		return false;
}
/*
OGLCheckError function, returns UNICODE bytes array with error message
*/
inline int OGLCheckError(LPWSTR strOut, LPCWSTR funcName, int bufSize = 512) {
	GLuint error = glGetError();
	switch(error) {
	case GL_NO_ERROR:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): No error has been recorded!\n",funcName);
		break;
	case GL_INVALID_ENUM:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): An unacceptable value is specified for an enumerated argument!\n",funcName);
		break;
	case GL_INVALID_VALUE:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): A numeric argument is out of range!\n",funcName);
		break;
	case GL_INVALID_OPERATION:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): The specified operation is not allowed in the current state!\n",funcName);
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): The framebuffer object is not complete!\n",funcName);
		break;
	case GL_OUT_OF_MEMORY:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): There is not enough memory left to execute the command!\n",funcName);
		break;
	case GL_STACK_UNDERFLOW:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): An attempt has been made to perform an operation that would cause an internal stack to underflow!\n",funcName);
		break;
	case GL_STACK_OVERFLOW:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): An attempt has been made to perform an operation that would cause an internal stack to overflow!\n",funcName);
		break;
	default:
		swprintf(strOut,bufSize,L"\nOpenGL Error (%s): Undefined error!\n",funcName);
		break;
	}
	return error;
}
#define FOR(a,lim) for(int a = 0; a < lim; a++)
#define FOReq(a,lim) for(int a = 0; a <= lim; a++)
#define sFOR(a,start,lim) for(int a = start; a < lim; a++)
#define sFOReq(a,start,lim) for(int a = start; a <= lim; a++)
#define rFOR(a,lim) for(int a = lim - 1; a >= 0; a--)

#endif