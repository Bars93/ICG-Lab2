#include "glModel.h"
#include <stdlib.h>
#include <sstream>
#include <strstream>
#define sizeWSTR(a) (sizeof((a))/sizeof(WCHAR))
#define sizeSTR(a) (sizeof((a))/sizeof(CHAR))

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
	modelMatr(NULL),
	normalDirect(1.0f)
{
	X = glm::vec3(1.0f,0.0f,0.0f);
	Y = glm::vec3(0.0f,1.0f,0.0f);
	Z = glm::vec3(0.0f,0.0f,1.0f);
	memset(&modelStates,0,sizeof(bitStates));
	modelStates.remake = 1;
	modelStates.redraw = 1;
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
			modelStates.useShadersflg = 1;
		}
		else {
			globRes = false;
			modelStates.useShadersflg = 0;
		}
		modelStates.useShadersflg  = 0;
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
			lookCamera(glm::vec3(11.00f, 12.00f, -4.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
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
	glMultMatrixf(&modelMatr[0][0]);
	/*if(useShadersflg)
	GLSL->useProgram();*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vnID);
	glNormalPointer(GL_FLOAT,0,(GLvoid*)0);
	glColor3ub(15,124,141);
	glDrawArrays(GL_QUADS,0,vertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glPopMatrix();
	glFlush();
	GLint err = glGetError();
}
glm::vec3 glModel::calculateNormal(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c) {
	return glm::normalize(glm::cross(c - a, b - a))*normalDirect;
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
	projMatr = glm::perspective(45.0f, aspect,  0.125f, 512.0f);
	if(!useShadersflg)
		glLoadMatrixf(&projMatr[0][0]);
}


///////////////////////////////////////////////////////////////////////////////
// rotate the camera
///////////////////////////////////////////////////////////////////////////////
void glModel::rotateCamera(int x, int y)
{
	if(mouseLeftDown)
	{
		// moving by x, by y (of window)
		GLfloat dx = GLfloat(x) - mouseX;
		GLfloat dy = GLfloat(y) - mouseY;
		// setting up sensetivity
		float Sensitivity = 1.00f;

		if(dx != 0)
		{
			// moving left/right
			GLfloat DeltaX = dx * Sensitivity;
			// rotate our basic vector's around physical Y
			X = glm::rotate(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Y = glm::rotate(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Z = glm::rotate(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		
		if(dy != 0)
		{
			// moving top/bottom
			GLfloat DeltaY = dy * Sensitivity;

			Y = glm::rotate(Y, DeltaY, X);
			Z = glm::rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = glm::cross(Z, X);
			}
		}
		Reference = Z * glm::length(Position);  
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
	if(delta < 0) {
		Position += Reference * 0.025f;
	}
	else if(delta > 0) {	
		Position -= Reference * 0.025f;
	}
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
bool glModel::loadDialog(LPCWSTR dialogTitle, LPWSTR filename) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFilter = L"*.OBJ";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeWSTR(filename);
	ofn.lpstrTitle = dialogTitle;
	ofn.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING 
		| OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;
	if(GetOpenFileName(&ofn) == NULL) {
		if(CommDlgExtendedError() == NULL) {
			MessageBox(handle,L"Some error with dialog init/run",L"GL_MODEL CLASS", MB_OK | MB_ICONEXCLAMATION);
		}
		else {
			MessageBox(handle,L"File not choose. You did cancel operation",L"GL_MODEL CLASS", MB_OK | MB_ICONEXCLAMATION);
		}
		return false;
	}
	return true;
}
#define closeFile(fp) \
	if(fp) { \
	fclose(fp); \
	fp = NULL; \
	} \
	else { \
	MessageBox(NULL,L"File already closed. closeFile error",L"Close file", MB_OK | MB_ICONERROR); \
}
void glModel::loadFileData() {
	LPCWSTR mode = L"r";
	WCHAR buf[FILENAME_MAX];
	FILE *fp = NULL;
	if(MessageBox(handle,L"Would you like to load Data files from?",L"ICG GL Lab-2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		buf[0] = L'\0';
		if(loadDialog(L"Slice file choice ...",buf)) {
			if(lstrlenW(buf) > 3) {
				fp = _wfopen(buf,mode);
				if(fp) {
					glm::vec2 sl_vert;
					closeFile(fp);
				}
			}
		}
	}
}