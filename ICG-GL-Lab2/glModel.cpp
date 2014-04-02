
#include "glModel.h"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glew32s.lib")

glModel::~glModel(void)
{
}
///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
glModel::glModel() : mouseLeftDown(false), mouseRightDown(false),
	cameraAngleX(0), cameraAngleY(0), cameraDistance(5)
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


	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glClearColor(0,0,0, 1);
	// 0 is near, 1 is far
	glEnable(GL_COLOR_LOGIC_OP);
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);
	glGenBuffers(1,&voID);
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
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		0.0f,  1.0f
	};
	// This will identify our vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, voID);

	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	// 1rst attribute buffer : vertices

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

	glDisableVertexAttribArray(0);
	// save the initial ModelView matrix before modifying ModelView matrix
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