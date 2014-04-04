
#include "glModel.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
glModel::glModel() : 
	mouseLeftDown(false), 
	mouseRightDown(false),
	cameraAngleX(0), 
	cameraAngleY(0), 
	cameraDistance(5),
	GLSL(NULL),
	useShadersflg(false)
{
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////

glModel::~glModel(void)
{
	delete[] vertBuf;
}
///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void glModel::init()
{
	glewInit();
	GLuint err = glGetError();
	if(err > 0) {

	}
	GLSL = new glShader(handle);
	if((program = GLSL->loadAndAttach("shaders\\ICG-Lab2.vert","shaders\\ICG-Lab2.frag")) != -1)
		useShadersflg = true;
	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glClearColor(255,255,255, 1);
	glEnable(GL_DEPTH_TEST);
	initBuffers(); 
}
void glModel::initBuffers() {
	vertBuf = new GLfloat[20*2];
	for(int i = 0; i < 40; i++) {
		vertBuf[i] = GLfloat(rand()%640);
	}
	GLubyte *colorBuf = new GLubyte[20*3];
	for(int i = 0; i < 60; i++) {
		colorBuf[i] = GLubyte(rand()%255);
	}
	glGenBuffers(1,&vbID);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*40, vertBuf, GL_STATIC_DRAW);
	int bufferSize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if(sizeof(GLfloat)*40 != bufferSize)
	{
		glDeleteBuffersARB(1, &vbID);
		vbID = 0;
		MessageBox(handle,L"VBO error",L"K.O",MB_OK);
	}
	glGenBuffers(1,&vcID);
	glBindBuffer(GL_ARRAY_BUFFER,vcID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)*60, colorBuf, GL_STATIC_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if(sizeof(GLubyte)*60 != bufferSize)
	{
		glDeleteBuffersARB(1, &vcID);
		vcID = 0;
		MessageBox(handle,L"VBO error",L"K.O",MB_OK);
	}
	if((attrLoc = glGetAttribLocation(program,"position")) < 0) {
		MessageBox(handle,L"position not found",L"Modeling GL", MB_OK | MB_ICONERROR);
	}
	delete[] colorBuf;
}
void glModel::draw()
{
	// clear buffer
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	GLSL->useProgram();
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glVertexPointer(2,GL_FLOAT,0,0);
	glVertexAttribPointer(attrLoc,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(attrLoc);
	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vcID);
	glColorPointer(3,GL_UNSIGNED_BYTE,0,(GLvoid*)0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,viID);
	glDrawArrays(GL_QUADS,0,20);
	glDisableVertexAttribArray(attrLoc);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glFlush();
	static GLint err = glGetError();
}
///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
//void glModel::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
//{
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
//}



///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport
///////////////////////////////////////////////////////////////////////////////
void glModel::setViewport(int w, int h)
{
	// set viewport to be the entire window
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,GLdouble(w),0.0,GLdouble(h),-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// draw 2D/3D scene
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// rotate the camera
///////////////////////////////////////////////////////////////////////////////
//void glModel::rotateCamera(int x, int y)
//{
//	if(mouseLeftDown)
//	{
//		cameraAngleY += (x - mouseX);
//		cameraAngleX += (y - mouseY);
//		mouseX = x;
//		mouseY = y;
//	}
//}



///////////////////////////////////////////////////////////////////////////////
// zoom the camera
///////////////////////////////////////////////////////////////////////////////
//void glModel::zoomCamera(int delta)
//{
//	if(mouseRightDown)
//	{
//		cameraDistance += (delta - mouseY) * 0.05f;
//		mouseY = delta;
//	}
//}