#pragma warning (disable : 4996)
#include "glModel.h"
#include <stdlib.h>
#include <cwchar>

glModel::glModel() :
	normalDirect(1.0f),
	hDevC(NULL)
{
	lightPos[mLight0] = glm::vec4(-10.0f, 10.0f, 2.0f, 1.0f);
	lightDifColor[mLight0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	X = glm::vec3(1.0f,0.0f,0.0f);
	Y = glm::vec3(0.0f,1.0f,0.0f);
	Z = glm::vec3(0.0f,0.0f,1.0f);
	ZeroMemory(&modelStates,sizeof(bitStates));
}

glModel::~glModel(void)
{
	glDeleteBuffers(bufCount,buffers);
	glDeleteVertexArrays(VAOcount,VAOs);
}
void glModel::initLightingMode() {
	// attribute locations
	MainWithLightShader.BindAttribLocation(0,"vPosition");
	MainWithLightShader.BindAttribLocation(1,"vColor");
	MainWithLightShader.BindAttribLocation(2,"normalVec");
}
void glModel::initDelightingMode() {
	MainWithLightShader.BindAttribLocation(0,"vPosition");
	MainWithLightShader.BindAttribLocation(1,"vColor");
}
void glModel::bindLightShaderVariables() {
	// setting attribute locations
	int nAttribs = 0;
	glGetProgramiv(ProgLight, GL_ACTIVE_ATTRIBUTES,&nAttribs);
	shaderAttribLocs[vertPosition] = MainWithLightShader.getAttribLocation("vPosition");
	shaderAttribLocs[vertColor] = MainWithLightShader.getAttribLocation("vColor");
	shaderAttribLocs[normalVec] = MainWithLightShader.getAttribLocation("normalVec");
	// setting uniform locations
	/*
	uniform mat4x4 MVMatrix;
	uniform mat4x4 NormMatrix;
	uniform mat4x4 MVPMatrix;
	*/
	shaderUniformLocs[MVMatr] = MainWithLightShader.getUniformLocation("ModelViewMatrix");
	shaderUniformLocs[MVPMatr] = MainWithLightShader.getUniformLocation("MVP");
	shaderUniformLocs[NormMatr] = MainWithLightShader.getUniformLocation("NormalMatrix");
}
void glModel::bindDelightShaderVariables() {


}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void glModel::checkReadyToDraw() {
	bitStates *ms = &modelStates;
	ms->readyToDraw = ms->loadPath & ms->loadSlice & ms->useShadersflg & ms->bufferBind;
}
bool glModel::init()
{
	GLuint error = OGLCheckError(errorString,L"Init",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		return false;
	}
	bool globRes = true;
	const shaderInfo curShaders[] = {
		{L"shaders\\ICG-Lab2.vert",GLSL_VERTEX},
		{L"shaders\\ICG-Lab2.frag",GLSL_FRAGMENT},
		{L"",GLSL_NONE},
		0
	};
	const shaderInfo curlightShaders[] = {
		{L"shaders\\ICG-Lab2_with_lighting.vert",GLSL_VERTEX},
		{L"shaders\\ICG-Lab2_with_lighting.frag",GLSL_FRAGMENT},
		{L"",GLSL_NONE},
		0
	};
	error = OGLCheckError(errorString,L"Init",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		return false;
	}
	MainShader.loadAndAttach(curShaders);
	MainWithLightShader.loadAndAttach(curlightShaders);
	initLightingMode();
	initDelightingMode();
	if(MainShader.link(&ProgLight)) 
		modelStates.useShadersflg = 1;
	else 
		modelStates.useShadersflg = 0;

	if(MainWithLightShader.link(&ProgDelight)) 
		modelStates.useShadersflg = 1;
	else 
		modelStates.useShadersflg = 0;
	if(MainWithLightShader.useProgram()) {
		modelStates.shaderInUse = 1;
	}
	//glClearColor(0.0,0.0,0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glPointSize(20.0f);
	error = OGLCheckError(errorString,L"Init",1024);
	if(MessageBox(handle,L"Загрузить данные из файлов?",L"ICG GL Lab-2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		loadFileData();
	}
	if(error > 0 || !modelStates.useShadersflg) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		globRes = false;
	}
	modelStates.readyToDraw = modelStates.loadSlice & modelStates.loadPath & modelStates.useShadersflg;
	lookCamera(glm::vec3(10.0f, 10.0f, -8.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
	if(modelStates.readyToDraw) {

		bindLightShaderVariables();

		if(!initBuffers()) {
			MessageBox(handle,L"Error buffer init",L"ICG GL Lab-2",MB_OK | MB_ICONEXCLAMATION);
			globRes = false;
		}
		checkReadyToDraw();
	}
	error = OGLCheckError(errorString,L"Init",1024);
	if(error > 0) {
		globRes = false;
	}
	return globRes;
}
void glModel::unbindAllShaders() {
	if(modelStates.shaderInUse) {
		glUseProgram(GL_ZERO);
		modelStates.shaderInUse = 0;
	}
}
void glModel::initLights() {

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(lightDifColor[mLight0]));
	//glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(lightPos[mLight0]));
}
void glModel::calcNormVectors() {
	const GLfloat normVScale = 3.0f;
	if(vertexCount == GL_ZERO)
		return;
	glm::vec3 dot1,dot2,dot3;
	for(GLuint i = 0; i < vertexCount; i+=3) {
		int n1 = i, n2 = i + 1, n3 = i + 2;
		dot1 = quadBuf[i].xyz();
		dot2 = quadBuf[i+1].xyz();
		dot3 = quadBuf[i+2].xyz();
		normalBuf.push_back(calculateNormal(dot1,dot2,dot3));
		normalBuf.push_back(calculateNormal(dot1,dot2,dot3));
		normalBuf.push_back(calculateNormal(dot1,dot2,dot3));
	}
	for(GLuint i = 0; i < vertexCount; i++) {
		normVBuf.push_back(quadBuf[i].xyz());
		normVBuf.push_back(quadBuf[i].xyz()+(normalBuf[i]*normVScale));
	}
	normVecCount = normVBuf.size();
}
inline float randFloat(float a, float b) {
	return ((b-a)*((float)rand()/RAND_MAX))+a;
}
bool glModel::initBuffers() {
	bool globRes = true;
	if(modelStates.loadPath && modelStates.loadSlice) {
		if(modelStates.useShadersflg) {
			if(!modelStates.lighting) {

			}

		}
		vertexCount = userPath.size();
		for(GLuint i = 0; i < vertexCount; i++)
			colorBuf.push_back(glm::vec3(randFloat(0.0f,1.0f),randFloat(0.0f,1.0f),randFloat(0.0f,1.0f)));
		//calcNormVectors();
		int bufferSize = 0;
		glGenBuffers(bufCount,buffers);

		//glBindBuffer(GL_ARRAY_BUFFER,buffers[vnVecBufID]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normVecCount, &normVBuf[0], GL_STATIC_DRAW);
		//bufferSize = 0;
		//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		//if(sizeof(glm::vec3)*normVecCount != bufferSize)
		//{
		//	glDeleteBuffersARB(1, &buffers[vnVecBufID]);
		//	buffers[vnVecBufID] = 0;
		//	MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		//	modelStates.showNormals = 0;
		//}

		glBindBuffer(GL_ARRAY_BUFFER,buffers[colorBufID]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexCount, &colorBuf[0], GL_STATIC_DRAW);
		bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if(sizeof(glm::vec3)*vertexCount != bufferSize)
		{
			glDeleteBuffersARB(1, &buffers[colorBufID]);
			buffers[colorBufID] = 0;
			MessageBox(handle,L"VBO error",L"K.O",MB_OK);
			modelStates.showNormals = 0;
		}
		glBindBuffer(GL_ARRAY_BUFFER,buffers[vbID]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*vertexCount, &userPath[0], GL_STATIC_DRAW);
		bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if(sizeof(glm::vec4)*vertexCount != bufferSize)
		{
			glDeleteBuffersARB(1, &buffers[vbID]);
			buffers[vbID] = 0;
			MessageBox(handle,L"VBO error",L"K.O",MB_OK);
			globRes = false;
		}
		/*glBindBuffer(GL_ARRAY_BUFFER,buffers[vnID]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexCount, &normalBuf[0],GL_STATIC_DRAW);
		bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if(sizeof(glm::vec3)*vertexCount != bufferSize)
		{
		glDeleteBuffersARB(1, &buffers[vnID]);
		buffers[vnID] = 0;
		MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		globRes = false;
		}
		*/
		glGenVertexArrays(VAOcount,VAOs);
		RebindShaderAttributes();
		colorBuf.clear();
	}
	else {
		globRes = false;
	}
	if(globRes)
		modelStates.bufferBind = 1;
	else
		modelStates.bufferBind = 0;
	return globRes;
}
void glModel::swapDevBuffers() {
	glFlush();
	if(hDevC)
		SwapBuffers(hDevC);
	else {
		MessageBox(handle,L"Error switch buffers. DoubleBuffer error!",L"GL_MODEL CLASS", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(EXIT_FAILURE);
	}

}
void glModel::RebindShaderAttributes() {
	glBindVertexArray(VAOs[quadsVA]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[vbID]);
	glEnableVertexAttribArray(/*shaderAttribLocs[vertPosition]*/0);
	glVertexAttribPointer(/*shaderAttribLocs[vertPosition]*/0,3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);

	//glBindBuffer(GL_ARRAY_BUFFER,buffers[vnID]);
	//glVertexAttribPointer(shaderAttribLocs[normalVec],3,GL_FLOAT,GL_TRUE,0,(GLvoid*)0);
	//glEnableVertexAttribArray(shaderAttribLocs[normalVec]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[colorBufID]);
	glEnableVertexAttribArray(/*shaderAttribLocs[vertColor]*/2);
	glVertexAttribPointer(/*shaderAttribLocs[vertColor]*/2,3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glBindVertexArray(VAOs[normVecVA]);

	glBindBuffer(GL_ARRAY_BUFFER,buffers[vnVecBufID]);
	glVertexAttribPointer(shaderAttribLocs[vertPosition],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(shaderAttribLocs[vertPosition]);
	glBindVertexArray(GL_ZERO);

}
void glModel::draw()
{
	// clear buffer
	if(modelStates.lighting) {
		initLights();
	}
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(modelStates.readyToDraw) {
		if(modelStates.useShadersflg) {
			if(!modelStates.shaderInUse) {
				if(MainWithLightShader.useProgram())
					modelStates.shaderInUse = 1;
			}
		}
		if(modelStates.shaderInUse) {
			bindLightShaderVariables();
			MV = modelMatr * ViewMatrix;
			normalMatrix = glm::transpose(glm::inverse(MV));
			MVP = projMatr * MV;
			glUniformMatrix4fv(shaderUniformLocs[MVPMatr],1,GL_FALSE,glm::value_ptr(MVP));
			glUniformMatrix4fv(shaderUniformLocs[NormMatr],1,GL_FALSE,glm::value_ptr(normalMatrix));
		}
		glBindVertexArray(VAOs[quadsVA]);
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		glBindVertexArray(GL_ZERO);
		if(modelStates.useShadersflg) {
			glUseProgram(0);
			modelStates.shaderInUse = 0;
		}

	}
	swapDevBuffers();
	GLint error = OGLCheckError(errorString,L"draw",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
	}
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
	this->width = GLfloat(w) - 180.0f;
	this->height = GLfloat(h);
	GLfloat aspect = width / height;
	// set viewport to be the entire window'
	glViewport(0,0,w-180,h);
	modelMatr = glm::translate(glm::vec3(0.0f,5.0f,0.0f));
	projMatr = glm::perspective(45.0f, aspect,  0.001f, 1000.0f);
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
		if(CommDlgExtendedError() == FALSE) {
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
	userPath.clear();
	slice.clear();
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
	if(modelStates.loadSlice)
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