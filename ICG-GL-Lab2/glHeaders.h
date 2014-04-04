#ifndef GLHEADERS_H
#define GLHEADERS_H
#include <stdlib.h>
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
#endif