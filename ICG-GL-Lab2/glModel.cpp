#pragma warning (disable : 4996)
#include "glModel.h"
#include <stdlib.h>
#include <wchar.h>
// Debug output function
static void APIENTRY debugOutput
	(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	GLvoid* userParam
	)
{
	FILE* f;
	f = fopen("debug_output.txt","a");
	if(f)
	{
		char debSource[32], debType[32], debSev[32];
		bool Error(false);

		if(source == GL_DEBUG_SOURCE_API_ARB)
			strcpy(debSource, "OpenGL");
		else if(source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
			strcpy(debSource, "Windows");
		else if(source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
			strcpy(debSource, "Shader Compiler");
		else if(source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
			strcpy(debSource, "Third Party");
		else if(source == GL_DEBUG_SOURCE_APPLICATION_ARB)
			strcpy(debSource, "Application");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
			strcpy(debSource, "Other");
		else
			assert(0);

		if(type == GL_DEBUG_TYPE_ERROR)
			strcpy(debType, "error");
		else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
			strcpy(debType, "deprecated behavior");
		else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
			strcpy(debType, "undefined behavior");
		else if(type == GL_DEBUG_TYPE_PORTABILITY)
			strcpy(debType, "portability");
		else if(type == GL_DEBUG_TYPE_PERFORMANCE)
			strcpy(debType, "performance");
		else if(type == GL_DEBUG_TYPE_OTHER)
			strcpy(debType, "message");
		else if(type == GL_DEBUG_TYPE_MARKER)
			strcpy(debType, "marker");
		else if(type == GL_DEBUG_TYPE_PUSH_GROUP)
			strcpy(debType, "push group");
		else if(type == GL_DEBUG_TYPE_POP_GROUP)
			strcpy(debType, "pop group");
		else
			assert(0);

		if(severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		{
			strcpy(debSev, "high");
			//Error = true;
		}
		else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
			strcpy(debSev, "medium");
		else if(severity == GL_DEBUG_SEVERITY_LOW_ARB)
			strcpy(debSev, "low");
		else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			strcpy(debSev, "notification");
		else
			assert(0);

		fprintf(f,"%s: %s(%s) %d: %s\n", debSource, debType, debSev, id, message);
		assert(!Error);
		fclose(f);
	}
}
glModel::glModel() :
	normalDirect(1.0f),
	hDevC(NULL)
{
	ZeroMemory(&shaderAttribLocs,sizeof(shaderAttribLocs));
	ZeroMemory(&shaderUniformLocs,sizeof(shaderUniformLocs));
	materialP.refAmbient = glm::vec3(0.2313f);
	materialP.refDiffuse = glm::vec3(0.2775f);
	materialP.refSpecular = glm::vec3(0.7739f);
	materialP.Shininess = 90.0f;
	lightsP[mLight0].ambient = glm::vec3(0.6f);
	lightsP[mLight0].diffuse = glm::vec3(1.0f);
	lightsP[mLight0].specular = glm::vec3(0.5f);
	lightsP[mLight0].position = glm::vec4(100.0f,100.0f,-20.0f,1.0f);
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

}
void glModel::bindLightShaderVariables() {
	// setting attribute locations
	int nAttribs = 0;
	glGetProgramiv(ProgLight, GL_ACTIVE_ATTRIBUTES,&nAttribs);
	shaderAttribLocs[vertPosition] = MainShader.getAttribLocation("vPosition");
	shaderAttribLocs[normalVec] = MainShader.getAttribLocation("vNormal");
	// setting uniform locations
	shaderUniformLocs[CameraPos] = MainShader.getUniformLocation("CameraPosition");
	shaderUniformLocs[light0position] = MainShader.getUniformLocation("light0.position");
	shaderUniformLocs[light0ambient] = MainShader.getUniformLocation("light0.ambient");
	shaderUniformLocs[light0diffuse] = MainShader.getUniformLocation("light0.diffuse");
	shaderUniformLocs[light0specular] = MainShader.getUniformLocation("light0.specular");
	shaderUniformLocs[matAmbient] = MainShader.getUniformLocation("material.refAmbient");
	shaderUniformLocs[matDiffuse] = MainShader.getUniformLocation("material.refDiffuse");
	shaderUniformLocs[matSpecular] = MainShader.getUniformLocation("material.refSpecular");
	shaderUniformLocs[matShininess] = MainShader.getUniformLocation("material.Shininess");
	shaderUniformLocs[vertColor3f] = MainShader.getUniformLocation("vColor");
	shaderUniformLocs[turnLight] = MainShader.getUniformLocation("turnLight");
	NormalVecShader.useProgram();
	NDshaderAttribs[NDvertPosition] = NormalVecShader.getAttribLocation("vPosition");
	NDshaderAttribs[NDnormalVec] = NormalVecShader.getAttribLocation("vertNormal");
	int attrNum, attrLen, curLength, curWritten, curLocation, curSize;
	GLenum attrType;
	std::vector<GLchar> allAList, attrname;
	std::vector<WCHAR> outStr;
	outStr.resize(500,'\0');
	glGetProgramiv(NShaderProg,GL_ACTIVE_ATTRIBUTES,&attrNum);
	glGetProgramiv(NShaderProg,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&attrLen);
	for(int i = 0; i < attrNum; i++) {
		attrname.resize(attrLen,'\0');
		glGetActiveAttrib(NShaderProg,i,attrLen,&curWritten,&curSize,&attrType,(GLchar*)&attrname[0]);
		curLocation = glGetAttribLocation(NShaderProg,&attrname[0]);
		attrname.resize(128,'\0');
		swprintf(&outStr[0],L"%S - location: %d",&attrname[0],curLocation);
		MessageBox(handle,&outStr[0],L"DBG",MB_OK | MB_ICONINFORMATION);
		attrname.clear();
	}
	NormalVecShader.turnOff();
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
	if(error == 0 && !glewIsSupported("GL_ARB_debug_output")) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageCallbackARB(debugOutput, NULL);
	}
	bool globRes = true;
	const shaderInfo MainShaderInfo[] = {
		{L"shaders\\ICG-Lab2.vert",GLSL_VERTEX},
		{L"shaders\\ICG-Lab2.frag",GLSL_FRAGMENT},
		{L"",GLSL_NONE},
		0
	};
	const shaderInfo NdrawInfo[] = {
		{L"shaders\\NormalsDraw.vert",GLSL_VERTEX},
		{L"shaders\\NormalsDraw.geom",GLSL_GEORMETRY},
		{L"shaders\\NormalsDraw.frag",GLSL_FRAGMENT},
		{L"",GLSL_NONE},
		0
	};
	error = OGLCheckError(errorString,L"Init",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		return false;
	}
	MainShader.loadAndAttach(MainShaderInfo);
	NormalVecShader.loadAndAttach(NdrawInfo);
	//MainShader.setProgramParameteri(GL_PROGRAM_SEPARABLE,GL_TRUE);
	if(MainShader.link(&ProgLight) && NormalVecShader.link(&NShaderProg)) 
		modelStates.useShadersflg = 1;
	else 
		modelStates.useShadersflg = 0;

	MainShader.printProgramLog();
	NormalVecShader.printProgramLog();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	error = OGLCheckError(errorString,L"Init",1024);
	glGenVertexArrays(VAOcount,VAOs);
	glGenBuffers(bufCount,buffers);
	if(MessageBox(handle,L"Загрузить данные из файлов?",L"ICG GL Lab-2", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		loadFileData();
	}
	if(error > 0 || !modelStates.useShadersflg) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		globRes = false;
	}
	modelStates.readyToDraw = modelStates.loadSlice & modelStates.loadPath & modelStates.useShadersflg;
	lookCamera(glm::vec3(50.0f, 50.0f, -8.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
	if(modelStates.readyToDraw) {
		//MainShader.setPipelineStages(GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT);
		MainShader.useProgram();
		bindLightShaderVariables();
		error = OGLCheckError(errorString,L"Init",1024);
		if(!error && !initBuffers()) {
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
bool glModel::initBuffers() {
	bool globRes = true;
	GLuint error = 0U;
	if(modelStates.loadPath && modelStates.loadSlice) {
		vertexCount = drawPath.size();
		int bufferSize = 0;
		GLint UniformBufferOffset(0);
		GLuint sideBufSz = sidesVerteces.size()*sizeof(glm::vec3);
		GLuint vertBufSz = drawPath.size()*sizeof(glm::vec3);
		GLuint curBufOffset(0);
		glGetIntegerv(
			GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
			&UniformBufferOffset);

		GLint UniformBlockSize = glm::max(GLint(sizeof(matrices)), UniformBufferOffset);
		MainShader.useProgram();
		glBindVertexArray(VAOs[mainVA]);
		glBindBuffer(GL_UNIFORM_BUFFER,buffers[matrMapBufID]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,buffers[pathDrawID]);
		glBufferData(GL_ARRAY_BUFFER,vertBufSz*2+sideBufSz*2,NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,curBufOffset,vertBufSz, &drawPath[0]); // pass to gpu positions of faces
		curBufOffset += vertBufSz;
		glBufferSubData(GL_ARRAY_BUFFER,curBufOffset,sideBufSz,&sidesVerteces[0]); // pass to gpu position of sides
		curBufOffset += sideBufSz;
		glBufferSubData(GL_ARRAY_BUFFER,curBufOffset,vertBufSz, &normalBuf[0]); // pass to gpu faces points normals
		curBufOffset += vertBufSz;
		glBufferSubData(GL_ARRAY_BUFFER,curBufOffset,sideBufSz, &sidesNormals[0]); // pass to gpu sides points normals
		curBufOffset += sideBufSz;
		bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if(curBufOffset != bufferSize)
		{
			glDeleteBuffersARB(1, &buffers[pathDrawID]);
			buffers[pathDrawID] = 0;
			MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		}
		error = OGLCheckError(errorString,L"initBuffers",1024);
		RebindShaderAttributes();
		error = 0U;
		error = OGLCheckError(errorString,L"initBuffers",1024);
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
void glModel::RebindShaderAttributes() 
{
	GLint maxVAttr = -1;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&maxVAttr);
	GLuint sideBufSz = sidesVerteces.size()*sizeof(glm::vec3);
	GLuint vertBufSz = drawPath.size()*sizeof(glm::vec3);
	// bind buffers to Vertex array objects
	glBindVertexArray(VAOs[mainVA]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[pathDrawID]);
	glVertexAttribPointer(shaderAttribLocs[vertPosition],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(shaderAttribLocs[vertPosition]);
	glVertexAttribPointer(shaderAttribLocs[normalVec],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)(vertBufSz+sideBufSz));
	glEnableVertexAttribArray(shaderAttribLocs[normalVec]);
	glBindVertexArray(GL_ZERO);

}
void glModel::RebindNormalShaderAttributes() {
	GLuint sideBufSz = sidesVerteces.size()*sizeof(glm::vec3);
	GLuint vertBufSz = drawPath.size()*sizeof(glm::vec3);
	glBindVertexArray(VAOs[mainVA]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[pathDrawID]);
	glVertexAttribPointer(NDshaderAttribs[NDvertPosition],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(NDshaderAttribs[NDvertPosition]);
	glVertexAttribPointer(NDshaderAttribs[NDnormalVec],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)(vertBufSz+sideBufSz));
	glEnableVertexAttribArray(NDshaderAttribs[NDnormalVec]);
	glBindVertexArray(GL_ZERO);
}
void glModel::unbindShaderAttributes() {
	glDisableVertexAttribArray(shaderAttribLocs[vertPosition]);
	glDisableVertexAttribArray(shaderAttribLocs[normalVec]);

}
void glModel::unbindNormalShaderAttributes() {
	glDisableVertexAttribArray(NDshaderAttribs[NDvertPosition]);
	glDisableVertexAttribArray(1);
}
void glModel::draw()
{
	GLuint error = OGLCheckError(errorString,L"draw",1024);
	if(glIsEnabled(GL_SCISSOR_TEST)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	if(modelStates.readyToDraw) {
		if(modelStates.useShadersflg) {
			MainShader.useProgram();
			RebindShaderAttributes();
			matrices.ModelView = matrices.View * matrices.Model;
			matrices.ModelViewProjection = matrices.Projection * matrices.ModelView;
			matrices.Normal = glm::transpose(glm::inverse(matrices.ModelView));
			glBindBuffer(GL_UNIFORM_BUFFER, buffers[matrMapBufID]);
			sTransform* Pointer = (sTransform*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(sTransform),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			*Pointer = matrices;
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			// selected color
			glm::vec3 curColor(0.5f);
			glProgramUniform3fv(ProgLight,shaderUniformLocs[vertColor3f],1,glm::value_ptr(curColor));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[CameraPos],1,glm::value_ptr(Position));
			glProgramUniform4fv(ProgLight,shaderUniformLocs[light0position],1,glm::value_ptr(lightsP[mLight0].position));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[light0ambient],1,glm::value_ptr(lightsP[mLight0].ambient));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[light0diffuse],1,glm::value_ptr(lightsP[mLight0].diffuse));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[light0specular],1,glm::value_ptr(lightsP[mLight0].specular));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[matAmbient],1,glm::value_ptr(materialP.refAmbient));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[matDiffuse],1,glm::value_ptr(materialP.refDiffuse));
			glProgramUniform3fv(ProgLight,shaderUniformLocs[matSpecular],1,glm::value_ptr(materialP.refSpecular));
			glProgramUniform1f(ProgLight,shaderUniformLocs[matShininess],materialP.Shininess);
			error = OGLCheckError(errorString,L"draw",1024);
			if(error > 0) {
				MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
				return;
			}
		}

		glBindVertexArray(VAOs[mainVA]);

		glBindBufferBase(GL_UNIFORM_BUFFER, 1, buffers[matrMapBufID]);
		if(modelStates.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glUniform1i(shaderUniformLocs[turnLight],0);
			glDisable(GL_CULL_FACE);
		}
		else {
			if(modelStates.lighting) {
				glUniform1i(shaderUniformLocs[turnLight],1);
			}
			else {
				glUniform1i(shaderUniformLocs[turnLight],0);
			}
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		glDrawArrays(GL_TRIANGLE_FAN,vertexCount,sliceVertCount);
		glFrontFace(GL_CW);
		glDrawArrays(GL_TRIANGLE_FAN,vertexCount+sliceVertCount,sliceVertCount);
		glFrontFace(GL_CCW);
		MainShader.turnOff();
		glBindBuffer(GL_UNIFORM_BUFFER,GL_ZERO);
		unbindShaderAttributes();
		error = OGLCheckError(errorString,L"draw",1024);
		if(modelStates.showNormals) {
			NormalVecShader.useProgram();
			RebindNormalShaderAttributes();
			glBindVertexArray(VAOs[mainVA]);
			error = OGLCheckError(errorString,L"draw",1024);
			GLint loc = NormalVecShader.getUniformLocation("vColor");
			glUniform3fv(loc,1,glm::value_ptr(glm::vec3(1.0f)));
			loc = NormalVecShader.getUniformLocation("MVP");
			glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(matrices.ModelViewProjection));
			glDrawArrays(GL_POINTS,0,vertexCount + sliceVertCount*2);
			NormalVecShader.turnOff();
			unbindNormalShaderAttributes();
		}
		glBindVertexArray(GL_ZERO);
		//NormalVecShader.turnOff();

	}
	swapDevBuffers();
	error = OGLCheckError(errorString,L"draw",1024);
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
	GLfloat aspect(0.0f); 
	if(w >= h) {
		aspect = width / height;
	}
	else {
		aspect = height / width;
	}
	// set viewport to be the entire window
	GLclampf val = 240.0f/255.0f;
	glClearColor(val,val,val,1.0f);
	glDisable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	swapDevBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_SCISSOR_TEST);
	glViewport(0,0,w-180,h);
	glScissor(0,0,w-180,h);
	matrices.Model = glm::translate(glm::vec3(0.0f));
	matrices.Projection = glm::perspective(45.0f, aspect,  0.01f, 10000.0f);
}
glm::vec3 glModel::projectPoint(const glm::vec3 &point,glm::mat3 &rotMatr,glm::vec3 &translateVector) {
	glm::vec3 rotVec = rotMatr * point;
	return rotVec+glm::vec3(translateVector);
}
void glModel::calculateSurfsWithoutBreaks() {
	// clear buffers
	indices.clear();
	slicePath.clear();
	normalBuf.clear();
	drawPath.resize(0);
	normVBuf.resize(0);
	sidesVerteces.resize(0);
	sidesNormals.resize(0);
	totalPathLength = 0.0f;
	const GLfloat angle = 0.0f;
	glm::mat3 rotation, R;
	glm::vec3 translation;
	glm::vec3 v1,v2,v3,v4;
	glm::vec3 T0, T1, N0, N1, B0, B1;
	glm::vec3 A;
	GLfloat sqx, sqy, sqz;
	GLfloat COS, COSL, SIN;
	GLfloat xycosl, yzcosl, zxcosl;
	GLfloat xsin, ysin, zsin;	
	glm::vec3 nrm, nrm2, nrm3, nrm4;
	// fill sides buffers and calculate their normals

	FOR(n,pathSteps)
	{
		translation = glm::vec3(userPath[n].x, userPath[n].y, userPath[n].z);
		if(n == 0) {
			T0 = glm::normalize(glm::vec3(userPath[1].x - userPath[0].x , userPath[1].y - userPath[0].y, userPath[1].z - userPath[0].z));
			N0 = glm::normalize(glm::cross(T0,glm::vec3(0.0f,1.0f,0.0f)));
			B0 = glm::normalize(glm::cross(T0,N0));
			T1 = T0;
			N1 = N0;
			B1 = B0;
		}
		else if(n != (pathSteps-1)) {
			T1 = glm::vec3(userPath[n+1].x - userPath[n].x , userPath[n+1].y - userPath[n].y, userPath[n+1].z - userPath[n].z);
			T1 = glm::normalize(T1);
			A =  -glm::cross(T0,T1)/ (glm::abs(glm::length(T0))*glm::abs(glm::length(T1)));
			A = glm::normalize(A);
			sqx = A.x*A.x;
			sqy = A.y*A.y;
			sqz = A.z*A.z;
			COS = glm::dot(T0,T1);
			COSL = 1-COS;
			xycosl = A.x*A.y*COSL;
			yzcosl = A.y*A.z*COSL;
			zxcosl = A.x*A.z*COSL;
			SIN = sqrt(1-COS*COS);
			xsin = A.x * SIN;
			ysin = A.y * SIN;
			zsin = A.z * SIN;

			R = glm::mat3(
				glm::vec3(sqx+(1-sqx)*COS, xycosl+zsin, zxcosl-ysin),
				glm::vec3(xycosl-zsin, sqy+(1-sqy)*COS, yzcosl+xsin),
				glm::vec3(zxcosl+ysin, yzcosl-xsin, sqz+(1-sqz)*COS)
				);

			N1 = N0*R;
			B1 = glm::cross(T1,N1);
			T0=T1;
			N0=N1;
			B0=B1;

		}
		float a = n/(float (pathSteps))*angle;
		N1 = glm::rotate(N0, a, T1);
		B1 = glm::rotate(B0, a, T1);
		rotation = glm::mat3(T1, N1, B1);
		FOR(i,sliceVertCount) {
			v1 = projectPoint(glm::vec3(0, slice[i].y, slice[i].x), rotation, translation);
			slicePath.push_back(v1);
			GLint curVert = 0;

			curVert = n*sliceVertCount+i;
			// calculate draw way
			if(n != pathSteps - 1 ) {
				GLint begSlice = n*sliceVertCount;
				GLint begNextSlice = (n+1)*sliceVertCount;
				GLint endSlice = begNextSlice - 1;
				GLint endNextSlice = (n+2)*sliceVertCount - 1;
				indices.push_back((GLuint) (curVert - 1 < begSlice) ? endSlice : (curVert - 1));
				indices.push_back(curVert);
				indices.push_back(curVert+sliceVertCount);
				indices.push_back(curVert);
				indices.push_back((curVert+sliceVertCount+1) > endNextSlice ? begNextSlice : (curVert+sliceVertCount+1));
				indices.push_back(curVert+sliceVertCount);
			}

		}
	}
	vector<glm::vec3>::iterator rangeBeg, rangeEnd;
	rangeEnd = slicePath.end();
	rangeBeg = rangeEnd - sliceVertCount;
	sidesVerteces.insert(sidesVerteces.begin(),rangeBeg,rangeEnd);
	GLuint indStart = 0;
	nrm = -calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	for(GLuint i = 0; i < sliceVertCount; i++) {
		sidesNormals.push_back(nrm);
	}
	rangeBeg = slicePath.begin();
	rangeEnd = rangeBeg + sliceVertCount;
	sidesVerteces.insert(sidesVerteces.end(),rangeBeg,rangeEnd);
	indStart = sliceVertCount;
	nrm = calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	for(GLuint i = 0; i < sliceVertCount; i++) {
		sidesNormals.push_back(nrm);
	}
	GLuint VCount = indices.size();
	for(GLuint i = 0; i < VCount; i++) {
		drawPath.push_back(slicePath[indices[i]]);
	}
	indices.clear();
	for(GLuint i = 0; i < VCount; i+=3) {
		v1 = drawPath[i];
		v2 = drawPath[i+1];
		v3 = drawPath[i+2];
		nrm = calculateNormal(v1,v3,v2);
		normalBuf.push_back(nrm);
		normalBuf.push_back(nrm);
		normalBuf.push_back(nrm);
	}
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
	matrices.View = glm::mat4x4(X.x,Y.x,Z.x,0.0f,X.y,Y.y,Z.y,0.0f,X.z,Y.z,Z.z,0.0f,-glm::dot(X,Position),-glm::dot(Y,Position),-glm::dot(Z,Position),1.0f);
}
void glModel::moveCameraByKB(int Keys) {
	float Speed = 5.0f;

	if(Keys & 0x40) Speed *= 2.0f;
	if(Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * 0.05f;

	glm::vec3 Forward = Z, Right = X, Up = Y;
	//glm::vec3 Up(0.0f, 1.0f, 0.0f);
	//glm::vec3 Right = X;
	//glm::vec3 Forward = glm::cross(Up, Right);

	Up *= Distance;
	Right *= Distance;
	Forward *= Distance;

	glm::vec3 Movement;

	if(Keys & 0x01) Movement -= Forward;
	if(Keys & 0x02) Movement += Forward;
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
	if(modelStates.loadPath && modelStates.loadSlice) {
		calculateSurfsWithoutBreaks();
	}
}