
#include "glModel.h"

glModel::~glModel(void)
{
}
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


///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void glModel::init()
{

	GLSL = new glShader(handle);
	if(GLSL->loadAndAttach("shaders\\ICG-Lab2.vert","shaders\\ICG-Lab2.frag"))
		useShadersflg = true;
	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glClearColor(0,0,0, 1);
	// 0 is near, 1 is far
	glEnable(GL_COLOR_LOGIC_OP);
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
	glOrtho(0,w,0,h,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// draw 2D/3D scene
///////////////////////////////////////////////////////////////////////////////
void glModel::draw()
{
	// clear buffer
	glClearColor(255,255,255,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	if(useShadersflg)
		GLSL->useProgram();
	glBegin(GL_POLYGON);
	if(!useShadersflg)
		glColor3ub(250,100,10);
	glVertex2i(100,150);
	glVertex2i(250,400);
	glVertex2i(0,200);
	glEnd();
	if(useShadersflg)
		glUseProgram(NULL);
	glPopMatrix();
}



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