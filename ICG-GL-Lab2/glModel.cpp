#pragma warning (disable : 4996)
#include "glModel.h"
#include <stdlib.h>
#include <cwchar>
#define sizeWSTR(a) (sizeof((a))/sizeof(WCHAR))
#define sizeSTR(a) (sizeof((a))/sizeof(CHAR))

///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////

glModel::glModel() : 
	GLSL(NULL),
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

	GLSL = new glShader(handle);
	if((program = GLSL->loadAndAttach("shaders\\ICG-Lab2.vert","shaders\\ICG-Lab2.frag")) != -1) {
		modelStates.useShadersflg = 1;
	}
	else {
		globRes = false;
		modelStates.useShadersflg = 0;
	}
	modelStates.useShadersflg  = 0;

	glClearColor(255,255,255, 1);
	glDepthFunc(GL_LEQUAL); 
	loadFileData();
	if(modelStates.loadPath && modelStates.loadSlice && initBuffers()) {
		lookCamera(glm::vec3(11.00f, 12.00f, -4.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
		initLights();
	}
	else {
		globRes = false;
	}
	GLuint err = glGetError();
	if(err > 0) {
		globRes = false;
	}
	return globRes;
}
void glModel::initLights() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f}; 
	GLfloat lightPos0[] = {10.0f, 10.0f, 2.0f, 1.0f}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glEnable(GL_COLOR_MATERIAL);
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
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(modelStates.lighting) {
		initLights();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(ViewMatrix));
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(modelMatr));
	/*if(useShadersflg)
	GLSL->useProgram();*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vbID);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vnID);
	glNormalPointer(GL_FLOAT,0,(GLvoid*)0);
	glColor3ub(24,101,155);
	glDrawArrays(GL_QUADS,0,vertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glPopMatrix();
	glFlush();
	glDisable(GL_LIGHTING);
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
	glViewport(0,0,w-180,h);
	modelMatr = glm::translate(glm::vec3(0.0f,5.0f,0.0f));
	glMatrixMode(GL_PROJECTION);
	projMatr = glm::perspective(45.0f, aspect,  0.125f, 512.0f);
	if(!modelStates.useShadersflg)
		glLoadMatrixf(glm::value_ptr(projMatr));
}


///////////////////////////////////////////////////////////////////////////////
// rotate the camera
///////////////////////////////////////////////////////////////////////////////
void glModel::rotateCamera(int x, int y)
{
	if(modelStates.mouseLeftDown)
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
		ViewMatrix = glm::mat4x4(X.x,Y.x,Z.x,0.0f,X.y,Y.y,Z.y,0.0f,X.z,Y.z,Z.z,0.0f,-glm::dot(X,Position),-glm::dot(Y,Position),-glm::dot(Z,Position),1.0f);
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
	static WCHAR cwd[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,cwd);
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFilter = L"Object Data Files\0*.OBJ\0\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = FILENAME_MAX;
	ofn.lpstrTitle = dialogTitle;
	ofn.lpstrInitialDir = cwd;
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
	} 
void glModel::loadFileData() {
	LPCWSTR mode = L"r";
	GLfloat min;
	GLint num = -1, k = 0;
	glm::vec2 vert;
	WCHAR buf[FILENAME_MAX];
	FILE *fp = NULL;
	if(MessageBox(handle,L"Would you like to load Data files from?",L"ICG GL Lab-2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		buf[0] = L'\0';
		if(loadDialog(L"Slice file choice ...",buf)) {
			if(lstrlenW(buf) > 5) {
				fp = _wfopen(buf,mode);
				if(fp) {
					while(!feof(fp)) {
						GLfloat x;
						fgetws(buf,FILENAME_MAX,fp);
						if(buf[0] == btowc('v') && buf[1] == btowc(' ')) {
							swscanf(buf+2,L"%f %f %f", &x, &vert.x, &vert.y);
							slice.push_back(vert);
							if(num == -1) {
								min = vert.x;
								num = 0;
							}
							else if(abs(vert.x - min) < 1e-7) {
								min = vert.x;
								num = k;
							}
							++k;
						}
					}
					sliceVertCount = slice.size();
					sliceSize = 40;
					modelStates.loadSlice = 1;
					closeFile(fp);
				}
			}
		} 
		// calculating normals direction
		{
			glm::vec3 v1, v2, v3;
			int n1 = num - 1;
			if(n1 < 0) {
				n1+=sliceVertCount;
			}
			int n2 = num;
			int n3 = num+1;
			n3 %= sliceVertCount;
			v1 = glm::vec3(0,slice[n1].y,slice[n2].x);
			v2 = glm::vec3(0,slice[n2].y,slice[n2].x);
			v3 = glm::vec3(0,slice[n3].y,slice[n3].x);
			if(calculateNormal(v1,v2,v3).x >= 0.0f) {
				normalDirect = -1.0f;
			}
			else {
				normalDirect = 1.0f;
			}
		}
		buf[0] = L'\0';
		if(loadDialog(L"Path file choice ...",buf)) {
			if(lstrlenW(buf) > 5) {
				fp = _wfopen(buf,mode);
				if(fp) {
					while(!feof(fp)) {
						glm::vec3 path_vert;
						fgetws(buf,FILENAME_MAX,fp);
						if(buf[0] == btowc('v') && buf[1] == btowc(' ')) {
							swscanf(buf+2,L"%f %f %f",&path_vert.x,&path_vert.y,&path_vert.z);
							userPath.push_back(path_vert);
						}
					}
					pathSteps = userPath.size();
					modelStates.loadPath = 1;
					closeFile(fp);
				}
			}
		} 
	}
}