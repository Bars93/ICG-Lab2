#ifndef GLMODEL_H
#define GLMODEL_H
#include "glHeaders.h"
#include "glShader.h"
#include <vector>

class glModel
{
	bool mouseLeftDown;
	bool mouseRightDown;
	HWND handle;
	int mouseX;
	int mouseY;
	float cameraAngleX, cameraAngleY, cameraDistance;
	BOOL useShadersflg;
	glShader *GLSL;
	GLuint program;
	GLuint vbID, vcID; // buffers ID
	GLuint texColorBuf; // color buffer
	GLuint vertexCount;
	GLint colAttrib,attrLoc;
	GLfloat *vertBuf;
	GLubyte *colBuf;
public:
	void init();    // initialize OpenGL states
	void initBuffers();
	//void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
	void setViewport(int width, int height);
	void draw();

	void setMouseLeft(bool flag) { mouseLeftDown = flag; };
	void setMouseRight(bool flag) { mouseRightDown = flag; };
	void setMousePosition(int x, int y) { mouseX = x; mouseY = y; };

	//void rotateCamera(int x, int y);
	//void zoomCamera(int dist);
	glModel(void);
	~glModel(void);
	// set functions
	void setWinHandle(HWND _handle) { handle = _handle; }
};
#endif
