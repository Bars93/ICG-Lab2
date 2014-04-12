#include "glModel.h"
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
glModel::glModel() : 
	mouseLeftDown(false), 
	mouseRightDown(false),
	GLSL(NULL),
	useShadersflg(false),
	viewMatr(NULL),
	viewMatrInverse(NULL),
	modelMatr(NULL)
{
	X = glm::vec3(1.0f,0.0f,0.0f);
	Y = glm::vec3(0.0f,1.0f,0.0f);
	Z = glm::vec3(0.0f,0.0f,1.0f);

	Position = glm::vec3(0.0f,0.0f,0.5f);
	Reference = glm::vec3(0.0f);
	setMatrPtrs(&ViewMatrix,NULL);
}

void glModel::setMatrPtrs(glm::mat4x4 *_viewMatr, glm::mat4x4 *_viewMatrInv) {
	viewMatr = _viewMatr;
	viewMatrInverse = _viewMatrInv;

}

///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////

glModel::~glModel(void)
{
	glDeleteBuffers(1,&vbID);
	glDeleteBuffers(1,&vnID);
}
///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
bool glModel::init()
{
	bool globRes = true;
	if(glewInit() != GLEW_OK) {
		globRes = false;
	}
	if(globRes) {
		GLSL = new glShader(handle);
		if((program = GLSL->loadAndAttach("shaders\\ICG-Lab2.vert","shaders\\ICG-Lab2.frag")) != -1) {
			useShadersflg = true;
		}
		else {
			globRes = false;
			useShadersflg = false;
		}
		useShadersflg = false;
		// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
		glClearColor(255,255,255, 1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f}; 
		GLfloat lightPos0[] = {10.0f, 10.0f, 2.0f, 1.0f}; 
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glEnable(GL_COLOR_MATERIAL);
		glDepthFunc(GL_LEQUAL); 
		if(initBuffers()) {
			lookCamera(glm::vec3(2.00f, 4.00f, -2.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
		}
		else {
			globRes = false;
		}

	}
	GLuint err = glGetError();
	if(err > 0) {
		globRes = false;
	}
	return globRes;
}
bool glModel::initBuffers() {
	bool globRes = true;
	// init cube and normals
	quadBuf.push_back(glm::vec3( 1.0f, 1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, 1.0f,  1.0f));
	quadBuf.push_back(glm::vec3( 1.0f, 1.0f,  1.0f));
	// normals side 1
	normalBuf.push_back(glm::vec3(0.0f,1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,1.0f,0.0f));
	// Bottom face (y = -1.0f)
	quadBuf.push_back(glm::vec3( 1.0f, -1.0f,  1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f,  1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
	quadBuf.push_back(glm::vec3( 1.0f, -1.0f, -1.0f));
	// normals side = 2;
	normalBuf.push_back(glm::vec3(0.0f,-1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,-1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,-1.0f,0.0f));
	normalBuf.push_back(glm::vec3(0.0f,-1.0f,0.0f));
	// Front face  (z = 1.0f)
	quadBuf.push_back(glm::vec3( 1.0f,  1.0f, 1.0f));
	quadBuf.push_back(glm::vec3(-1.0f,  1.0f, 1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
	quadBuf.push_back(glm::vec3( 1.0f, -1.0f, 1.0f));
	// normals side 3
	normalBuf.push_back(glm::vec3(0.0f,0.0f,1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,1.0f));
	// Back face (z = -1.0f)
	quadBuf.push_back(glm::vec3( 1.0f, -1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f,  1.0f, -1.0f));
	quadBuf.push_back(glm::vec3( 1.0f,  1.0f, -1.0f));
	// normals side 4
	normalBuf.push_back(glm::vec3(0.0f,0.0f,-1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,-1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,-1.0f));
	normalBuf.push_back(glm::vec3(0.0f,0.0f,-1.0f));

	// Left face (x = -1.0f)
	quadBuf.push_back(glm::vec3(-1.0f,  1.0f,  1.0f));
	quadBuf.push_back(glm::vec3(-1.0f,  1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(-1.0f, -1.0f,  1.0f));
	// normals side 5
	normalBuf.push_back(glm::vec3(-1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(-1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(-1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(-1.0f,0.0f,0.0f));

	// Right face (x = 1.0f)
	quadBuf.push_back(glm::vec3(1.0f,  1.0f, -1.0f));
	quadBuf.push_back(glm::vec3(1.0f,  1.0f,  1.0f));
	quadBuf.push_back(glm::vec3(1.0f, -1.0f,  1.0f));
	quadBuf.push_back(glm::vec3(1.0f, -1.0f, -1.0f));
	// normals side 6
	normalBuf.push_back(glm::vec3(1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(1.0f,0.0f,0.0f));
	normalBuf.push_back(glm::vec3(1.0f,0.0f,0.0f));
	vertexCount = quadBuf.size();
	glGenBuffers(1,&vbID);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexCount, &quadBuf[0], GL_STATIC_DRAW);
	int bufferSize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if(sizeof(glm::vec3)*vertexCount != bufferSize)
	{
		glDeleteBuffersARB(1, &vbID);
		vbID = 0;
		MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		globRes = false;
	}
	glGenBuffers(1,&vnID);
	glBindBuffer(GL_ARRAY_BUFFER,vnID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexCount, &normalBuf[0],GL_STATIC_DRAW);
	bufferSize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if(sizeof(glm::vec3)*vertexCount != bufferSize)
	{
		glDeleteBuffersARB(1, &vbID);
		vbID = 0;
		MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		globRes = false;
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
	quadBuf.clear();
	return globRes;
}
void glModel::draw()
{
	// clear buffer
	static bool first = false;
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&ViewMatrix[0][0]);
	glPushMatrix();
	if(true)
	{
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f,1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f,1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f,0.1f,1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f,0.1f,1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f,-0.1f,1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);

		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);

		float d = 50.0f;

		for(float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}

		glEnd();
	}
	glMultMatrixf(&modelMatr[0][0]);
	/*if(useShadersflg)
	GLSL->useProgram();*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vnID);
	glNormalPointer(GL_FLOAT,0,(GLvoid*)0);
	glColor3ub(15,124,140);
	glDrawArrays(GL_QUADS,0,vertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glPopMatrix();
	glFlush();
	GLint err = glGetError();
}
///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void glModel::setCamera(GLdouble posX, GLdouble posY, GLdouble posZ, GLdouble targetX, GLdouble targetY, GLdouble targetZ)
{
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

void glModel::lookCamera(const glm::vec3 &pos, const glm::vec3 &ref, bool RotateAroundReference) {

	this->Position = pos;
	this->Reference = ref;

	Z = glm::normalize(Position - Reference);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	calculateViewMatrix();
}

///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport
///////////////////////////////////////////////////////////////////////////////
void glModel::setViewport(int w, int h)
{
	if(h == 0) h = 1;
	this->width = GLfloat(w);
	this->height = GLfloat(h);
	GLfloat aspect = width / height;
	// set viewport to be the entire window
	glViewport(0,0,w,h);
	//glMatrixMode(GL_MODELVIEW);
	modelMatr = glm::translate(glm::vec3(0.0f,5.0f,0.0f));
	//glLoadMatrixf(&modelMatr[0][0]);
	glMatrixMode(GL_PROJECTION);
	projMatr = glm::perspective(45.0f, width / height,  0.005f, 512.0f);
	if(!useShadersflg)
		glLoadMatrixf(&projMatr[0][0]);
	/*
	gluPerspective(45.0f, aspect, 1.0f, -1.0f);
	setCamera(-2,-2,-2,0.0,0.0,0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
}


///////////////////////////////////////////////////////////////////////////////
// rotate the camera
///////////////////////////////////////////////////////////////////////////////
void glModel::rotateCamera(int x, int y)
{
	if(mouseLeftDown)
	{
		GLfloat dx = GLfloat(x) - mouseX;
		GLfloat dy = GLfloat(y) - mouseY;
		float Sensitivity = 1.00f;

		Position -= Reference;

		if(dx != 0)
		{
			GLfloat DeltaX = dx * Sensitivity;

			X = glm::rotate(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Y = glm::rotate(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Z = glm::rotate(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if(dy != 0)
		{
			GLfloat DeltaY = dy * Sensitivity;

			Y = glm::rotate(Y, DeltaY, X);
			Z = glm::rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = glm::cross(Z, X);
			}
		}
		Position = Reference + Z * glm::length(Position);
		calculateViewMatrix();
		mouseX = GLfloat(x);
		mouseY = GLfloat(y);
	}
}



///////////////////////////////////////////////////////////////////////////////
// zoom the camera
///////////////////////////////////////////////////////////////////////////////
void glModel::zoomCamera(int delta)
{

	Position -= Reference;
	if(delta < 0 && glm::length(Position) < 500.0f) {
		Position += Position * 0.25f;
	}
	if(delta > 0 && glm::length(Position) > 0.05f) {
		Position -= Position * 0.25f;
	}
	Position += Reference;
	calculateViewMatrix();
}
// Calculate view matrix
void glModel::calculateViewMatrix() {
	if(viewMatr != NULL) {
		*viewMatr = glm::mat4x4(X.x,Y.x,Z.x,0.0f,X.y,Y.y,Z.y,0.0f,X.z,Y.z,Z.z,0.0f,-glm::dot(X,Position),-glm::dot(Y,Position),-glm::dot(Z,Position),1.0f);
		if(viewMatrInverse != NULL) {
			*viewMatrInverse = glm::inverse(*viewMatr);
		}
	}
}
void glModel::moveCameraByKB(int Keys) {

	float Speed = 5.0f;

	if(Keys & 0x40) Speed *= 2.0f;
	if(Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * 0.05f;

	glm::vec3 Up(0.0f, 1.0f, 0.0f);
	glm::vec3 Right = X;
	glm::vec3 Forward = glm::cross(Up, Right);

	Up *= Distance;
	Right *= Distance;
	Forward *= Distance;

	glm::vec3 Movement;

	if(Keys & 0x01) Movement += Forward;
	if(Keys & 0x02) Movement -= Forward;
	if(Keys & 0x04) Movement -= Right;
	if(Keys & 0x08) Movement += Right;
	if(Keys & 0x10) Movement += Up;
	if(Keys & 0x20) Movement -= Up;

	Position += Movement;
	Reference += Movement;
	calculateViewMatrix();
}