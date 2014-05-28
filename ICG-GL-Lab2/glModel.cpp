#pragma warning (disable : 4996)
#include "glModel.h"
#include <cstdlib>
#include <wchar.h>
#include <cmath>
#include <cstring>
#include <algorithm>
using namespace std;
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
	materialP.refAmbient = glm::vec3(0.2f);
	materialP.refDiffuse = glm::vec3(1.0f);
	materialP.refSpecular = glm::vec3(0.0f);
	materialP.emission = glm::vec3(0.0f);
	materialP.Shininess = 10.0f;
	lightsP[mLight0].attenuation_constant = 1.0f;
	lightsP[mLight0].attenuation_linear = 0.05f;
	lightsP[mLight0].attenuation_quadratic = 0.0025f;
	lightsP[mLight0].ambient = glm::vec3(1.0f);
	lightsP[mLight0].diffuse = glm::vec3(1.0f);
	lightsP[mLight0].specular = glm::vec3(1.0f);
	lightsP[mLight0].position = glm::vec4(-10.0f,90.0f,-200.0f,1.0f);
	X = glm::vec3(1.0f,0.0f,0.0f);
	Y = glm::vec3(0.0f,1.0f,0.0f);
	Z = glm::vec3(0.0f,0.0f,1.0f);
	ZeroMemory(&modelStates,sizeof(bitStates));
}

glModel::~glModel(void)
{
	glDeleteBuffers(bufCount,buffers);
	glDeleteVertexArrays(VAOcount,VAOs);
	glDeleteTextures(texCount,modelTextures);
}



void glModel::TexturePath() {
	texCoords.clear();
	getTexCoords(drawPath,texCoords,totalPathLength);
	getSideTexCoords(sidesVerteces,texCoords,userPath);
}
void glModel::TextureBPPath() {
	texCoords.clear();
	getTexCoords(drawPath,texCoords,totalBrokenPathLength);
	getSideTexCoords(sidesVerteces,texCoords,calcPath);
}
void glModel::getSideTexCoords(const vector<glm::vec3> &vertecies, vector<glm::vec2> &texCoords, vector<glm::vec3> &path) {
	GLfloat texU, texV;
	// get radius of circle, find max
	GLfloat maxRside1 = 0.0f, maxRside2 = 0.0f;
	vector<GLfloat> rads;
	for(int i = 0; i < vertecies.size() / 2; i++) {
		GLfloat R = glm::length(vertecies[i]-path[0]);
		rads.push_back(R);
		if(R > maxRside1)
			maxRside1 = R;
	}
	for(int i = vertecies.size() / 2; i < vertecies.size(); i++) {
		GLfloat R = glm::length(vertecies[i]-path[path.size()-1]);
		rads.push_back(R);
		if(R > maxRside2)
			maxRside2 = R;
	}
	const glm::vec2 texCentre(0.5f);
	glm::vec2 dir, tex;
	vector<glm::vec2> dirs;
	for(int i = 0; i < vertecies.size() / 2; i++) {
		dir = glm::normalize(slice[i]-texCentre);
		tex = texCentre + rads[i]/maxRside1 * 0.5f * dir;
		dirs.push_back(dir);
		texCoords.push_back(tex);
	}
	int base = vertecies.size() / 2;
	for(int i = base; i < vertecies.size(); i++) {
		tex = texCentre + rads[i]/maxRside2 * 0.5f * dirs[i-base];
		texCoords.push_back(tex);
	}
}
void glModel::getTexCoords(const vector<glm::vec3> &vertecies, vector<glm::vec2> &texCoords, GLfloat pathLen) {
	int vertCount = vertecies.size();
	glm::vec3 v1, v2, v3,v4;
	GLfloat lenWidth, lenHeight;
	GLint segInd(-1);
	GLfloat texX,texY = 1.0f, prevTexX = 0.0f, segSize = 0.0f, texYDown = 0.0f;
	for(int i = 0; i < vertCount; i+=6) {
		if(i%(6*sliceVertCount)==0) {
			texYDown = texY - GLint(texY);
			prevTexX = 0.0f;
			segSize = 0.0f;
			++segInd;
			for(int j = i; j < i + 6 * sliceVertCount; j+=6) {
				segSize += glm::length(drawPath[i+1]-drawPath[i+2]);
			}
			texY = pathLengthes[segInd] / pathLen;
			// clamping
			if(fabs(texY - segSize) > 1e-7) {
				texY = segSize;
			}
			if(fabs(texY) > 1e-7) {
				texY = 1.0f;
			}
		}
		v1 = vertecies[i]; // left-down
		v3 = vertecies[i+2]; // right-down
		v2 = vertecies[i+4]; // left-top
		lenWidth = glm::length(v2-v1);
		lenHeight = glm::length(v3-v1);
		texX = 1.0f * (lenWidth /segSize);
		if(prevTexX > 1.0000001f)
			prevTexX = 1.0f - texX;
		GLfloat newTexX = prevTexX + texX;
		// horizontal segment orientation, X - up, Y - right of side
		texCoords.push_back(glm::vec2(prevTexX,texYDown)); // left-down
		texCoords.push_back(glm::vec2(newTexX,texYDown+texY)); // right-top
		texCoords.push_back(glm::vec2(prevTexX,texYDown+texY)); // right-down
		texCoords.push_back(glm::vec2(prevTexX,texYDown)); //left-down
		texCoords.push_back(glm::vec2(newTexX,texYDown)); // left-top
		texCoords.push_back(glm::vec2(newTexX,texYDown+texY)); // right-top
		prevTexX += texX;
	}
}
void glModel::bindLightShaderVariables() {
	// setting attribute locations
	shaderAttribLocs[vertPosition] = MainShader.getAttribLocation("vPosition");
	shaderAttribLocs[normalVec] = MainShader.getAttribLocation("vNormal");
	// setting uniform locations
	if(MainShader.setUniform3fv("light0.specular",lightsP[mLight0].specular)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform4fv("light0.position",lightsP[mLight0].position)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform3fv("light0.ambient",lightsP[mLight0].ambient)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform3fv("light0.diffuse",lightsP[mLight0].diffuse)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }

	if(MainShader.setUniform1f("light0.attenuation_constant",lightsP[mLight0].attenuation_constant)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform1f("light0.attenuation_linear",lightsP[mLight0].attenuation_linear)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform1f("light0.attenuation_quadratic",lightsP[mLight0].attenuation_quadratic)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }

	if(MainShader.setUniform3fv("material.refAmbient",materialP.refAmbient)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform3fv("material.refDiffuse",materialP.refDiffuse)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform3fv("material.refSpecular",materialP.refSpecular)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	if(MainShader.setUniform1f("material.Shininess",materialP.Shininess)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }

	if(MainShader.setUniform3fv("material.emission",materialP.emission)) 
	{ MessageBox(handle,L"Error set uniform location",L"ICG GL Lab-2", MB_OK | MB_ICONERROR); }
	NDshaderAttribs[NDvertPosition] = NormalVecShader.getAttribLocation("vPosition");
	NDshaderAttribs[NDnormalVec] = NormalVecShader.getAttribLocation("vertNormal");
}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////
void glModel::checkReadyToDraw() {
	bitStates *ms = &modelStates;
	ms->readyToDraw = ms->useShadersflg && ms->loadPath && ( true  ||  ms->loadBreaks) && ms->bufferBind;
}
bool glModel::init()
{
	GLuint error = OGLCheckError(errorString,L"Init",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		return false;
	}
	//if(error == 0 && !glewIsSupported("GL_ARB_debug_output")) {
	//	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	//	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	//	glDebugMessageCallbackARB(debugOutput, NULL);
	//}
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
		globRes = false;
	}
	if(!MainShader.loadAndAttach(MainShaderInfo) ||!NormalVecShader.loadAndAttach(NdrawInfo)) {
		globRes = false;
	}
	//MainShader.setProgramParameteri(GL_PROGRAM_SEPARABLE,GL_TRUE);
	if(MainShader.link(&ProgLight) && NormalVecShader.link(&NShaderProg)) 
		modelStates.useShadersflg = 1;
	else {
		modelStates.useShadersflg = 0;
		globRes = false;
	}

	MainShader.printProgramLog();
	NormalVecShader.printProgramLog();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);
	glGenVertexArrays(VAOcount,VAOs);
	glGenBuffers(bufCount,buffers);
	error = OGLCheckError(errorString,L"init",1024);
	if(error > 0 || !modelStates.useShadersflg) {
		MessageBox(handle,errorString,L"ICG GL Lab2", MB_OK | MB_ICONERROR);
		globRes = false;
	}
	if(!initTextures(true,8))
		globRes = false;
	lookCamera(glm::vec3(70.0f, 50.0f, -50.00f), glm::vec3(0.0f, 0.0f, 0.0f),false);
	if(modelStates.useShadersflg) {
		//MainShader.setPipelineStages(GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT);
		MainShader.useProgram();
		bindLightShaderVariables();
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
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,&UniformBufferOffset);
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
		glBindBuffer(GL_ARRAY_BUFFER,buffers[texBufID]);
		GLuint texBufSz = sizeof(glm::vec2)*texCoords.size();
		GLuint texBufElCnt = texCoords.size();
		glBufferData(GL_ARRAY_BUFFER,texBufSz,&texCoords[0],GL_STATIC_DRAW);
		bufferSize = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		if(texBufSz != bufferSize)
		{
			glDeleteBuffersARB(1, &buffers[texBufID]);
			buffers[texBufID] = 0;
			MessageBox(handle,L"VBO error",L"K.O",MB_OK);
		}
		error = OGLCheckError(errorString,L"initBuffers",1024);
		RebindShaderAttributes(VAOs[mainVA],buffers[pathDrawID],drawPath.size(),sidesVerteces.size());

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
bool glModel::initBuffersBP() {
	bool globRes = true;
	if(modelStates.loadPath && modelStates.loadSlice && modelStates.loadBreaks) {

	}
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
void glModel::RebindShaderAttributes(GLuint &currentVAO_ID, GLuint &currentBufID, GLuint vertCount, GLuint sideVertCount) // *
{
	GLuint sideBufSz = sideVertCount*sizeof(glm::vec3);
	GLuint vertBufSz = vertCount*sizeof(glm::vec3);
	// bind buffers to Vertex array objects
	glBindVertexArray(currentVAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER,currentBufID);
	glVertexAttribPointer(shaderAttribLocs[vertPosition],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(shaderAttribLocs[vertPosition]);
	glVertexAttribPointer(shaderAttribLocs[normalVec],3,GL_FLOAT,GL_FALSE,0,(GLvoid*)(vertBufSz+sideBufSz));
	glEnableVertexAttribArray(shaderAttribLocs[normalVec]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers[texBufID]);
	GLuint loc = MainShader.getAttribLocation("texCoords");
	glVertexAttribPointer(loc,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(loc);
	glBindVertexArray(GL_ZERO);
}
void glModel::RebindNormalShaderAttributes(GLuint &currentVAO_ID, GLuint &currentBufID, GLuint vertCount, GLuint sideVertCount) {
	GLuint sideBufSz = sideVertCount*sizeof(glm::vec3);
	GLuint vertBufSz = vertCount*sizeof(glm::vec3);
	// bind buffers to Vertex array objects
	glBindVertexArray(currentVAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER,currentBufID);
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
		if(modelStates.reCalcNormals) {
			if(modelStates.smoothNormals) {
				smoothingNormals(normalBuf);
			}
			else {
				calculatingNormals(drawPath,normalBuf);
			}
			initBuffers();
			error = OGLCheckError(errorString,L"draw",1024);
			modelStates.reCalcNormals = 0;
		}
		if(modelStates.useShadersflg) {
			MainShader.useProgram();
			RebindShaderAttributes(VAOs[mainVA],buffers[pathDrawID],drawPath.size(),sidesVerteces.size());
			matrices.ModelView = matrices.View * matrices.Model;
			matrices.ModelViewProjection = matrices.Projection * matrices.ModelView;
			matrices.Normal = glm::transpose(glm::inverse(matrices.ModelView));
			glBindBuffer(GL_UNIFORM_BUFFER, buffers[matrMapBufID]);
			sTransform* Pointer = (sTransform*)glMapBufferRange(
				GL_UNIFORM_BUFFER, 0, sizeof(sTransform),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			*Pointer = matrices;
			glUnmapBuffer(GL_UNIFORM_BUFFER);
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
			MainShader.setUniform1i("turnLight",0);

			MainShader.setUniform3fv("vColor",glm::vec3(1.0f));
			glDisable(GL_CULL_FACE);
		}
		else {
			if(modelStates.lighting) {
				MainShader.setUniform1i("turnLight",1);
			}
			else {
				MainShader.setUniform3fv("vColor",materialP.refAmbient);
				MainShader.setUniform1i("turnLight",0);
			}
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
		if(modelStates.texMode) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,modelTextures[cobTex]);
			MainShader.setUniform1i("cobTex",0);
			MainShader.setUniform1i("textureOn",1);
		}
		else {
			MainShader.setUniform1i("textureOn",0);
		}
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		//glDisableVertexAttribArray(MainShader.getAttribLocation("texCoords"));
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
			RebindNormalShaderAttributes(VAOs[mainVA],buffers[pathDrawID],drawPath.size(),sidesVerteces.size());
			glBindVertexArray(VAOs[mainVA]);
			error = OGLCheckError(errorString,L"draw",1024);
			NormalVecShader.setUniform3fv("vColor",glm::vec3(1.0,0.0,0.0));
			GLint loc = NormalVecShader.getUniformLocation("MVP");
			glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(matrices.ModelViewProjection));
			glDrawArrays(GL_POINTS,0,vertexCount + sliceVertCount*2);
			NormalVecShader.turnOff();
			unbindNormalShaderAttributes();
		}
		glBindVertexArray(GL_ZERO);

	}
	swapDevBuffers();
	error = OGLCheckError(errorString,L"draw",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
	}
}

bool glModel::LoadTexture(LPCWSTR filename,FIBITMAP **dib, int &tWidth, int &tHeight, int &tBPP) {
	FREE_IMAGE_FORMAT fif;
	HMODULE hdl = GetModuleHandle(NULL);
	vector<WCHAR> full_path(MAX_PATH);
	GetModuleFileName(hdl,&full_path[0],MAX_PATH);

	wstring str(&full_path[0]);
	UINT t = str.rfind(L'\\');
	str.resize(++t);
	str = str + wstring(filename);
	fif = FreeImage_GetFIFFromFilenameU(str.c_str());
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilenameU(str.c_str());
	if(fif == FIF_UNKNOWN) {
		swprintf(errorString,L"FI error open file %S, type = %S",filename,FreeImage_GetFormatFromFIF(fif));
		MessageBox(handle,errorString, L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	if(dib && FreeImage_FIFSupportsReading(fif)) {
		*dib = FreeImage_LoadU(fif,str.c_str());
	}
	else if(!dib) {
		swprintf(errorString,L"FI error write data to dib, no address",filename,FreeImage_GetFormatFromFIF(fif));
		MessageBox(handle, errorString, L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	else {
		swprintf(errorString,L"FI error reading file %S, type = %S",filename,FreeImage_GetFormatFromFIF(fif));
		MessageBox(handle, errorString, L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	str.clear();
	full_path.clear();
	if(*dib != NULL) {
		int OriginalWidth = FreeImage_GetWidth(*dib);
		int OriginalHeight = FreeImage_GetHeight(*dib);

		tWidth = OriginalWidth;
		tHeight = OriginalHeight;

		if(tWidth == 0 || tHeight == 0)
		{
			FreeImage_Unload(*dib);
			return false;
		}

		tBPP = FreeImage_GetBPP(*dib);
		GLint maxTexSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTexSize);
		if(tWidth > maxTexSize) tWidth = maxTexSize;
		if(tHeight > maxTexSize) tHeight = maxTexSize;

		if(!GLEW_ARB_texture_non_power_of_two)
		{
			tWidth = 1 << (int)floor((log((float)tWidth) / log(2.0f)) + 0.5f); 
			tHeight = 1 << (int)floor((log((float)tHeight) / log(2.0f)) + 0.5f);
		}

		if(tWidth != OriginalWidth || tHeight != OriginalHeight)
		{
			FIBITMAP *rdib = FreeImage_Rescale(*dib, tWidth, tHeight, FILTER_BICUBIC);
			FreeImage_Unload(*dib);
			*dib = rdib;
		}


	}
	return true;
}
#define isPowOf2(x) !((x)&((x)-1))
bool glModel::initTextures(const GLboolean bGenMipMaps, GLint curAF) {
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif
	FIBITMAP *dib(NULL);
	BYTE *bDataPtr(NULL);
	const WCHAR *texName = L"textures\\cobblestones.tex.jpg";
	int texWidth, texHeight, texBPP;
	if(!LoadTexture(texName,&dib,texWidth,texHeight,texBPP)) {
		swprintf(errorString,1024,L"Texture \'%S\' loading failed!",texName);
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	bDataPtr = FreeImage_GetBits(dib);
	if(bDataPtr == NULL || texWidth == 0 || texHeight == 0) {
		return false;
	}
	GLenum dataFormat = texBPP == 32 ? GL_BGRA : texBPP == 24 ? GL_BGR : 0;
	if(dataFormat == 0) {
		return false;
	}
	glDeleteTextures(texCount,modelTextures);
	glGenTextures(texCount,modelTextures);
	glBindTexture(GL_TEXTURE_2D,modelTextures[cobTex]);
	if(GLEW_EXT_texture_filter_anisotropic && curAF > 1) {
		int maxAFval = 0;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&maxAFval);
		if(!isPowOf2(curAF)) {
			curAF = (int)ceil(log(double(curAF))/log(2.0)) + 1;
		}
		if(curAF > maxAFval)
			curAF = maxAFval;
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,curAF);
	}
	if(bGenMipMaps) {
		int num_mipmaps = (int)ceil(log(double(max(texWidth,texHeight)))/log(2.0)) + 1;
		if(GLEW_ARB_texture_storage) {
			glTexStorage2D(GL_TEXTURE_2D,num_mipmaps,GL_RGBA8,texWidth,texHeight);
			glTexSubImage2D(GL_TEXTURE_2D,0,0,0,texWidth,texHeight,dataFormat,GL_UNSIGNED_BYTE,bDataPtr);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,texWidth,texHeight,0,dataFormat,GL_UNSIGNED_BYTE,bDataPtr);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // trilinear filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	else {
		if(GLEW_ARB_texture_storage) {
			glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,texWidth,texHeight);
			glTexSubImage2D(GL_TEXTURE_2D,0,0,0,texWidth,texHeight,dataFormat,GL_UNSIGNED_BYTE,bDataPtr);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,texWidth,texHeight,0,dataFormat,GL_UNSIGNED_BYTE,bDataPtr);
		}
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // bilinear filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
	GLuint error = OGLCheckError(errorString,L"initTextures",1024);
	FreeImage_Unload(dib);
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
	return true;
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
	this->width = GLfloat(w) - 200.0f;
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
	glViewport(0,0,w-200,h);
	glScissor(0,0,w-200,h);
	matrices.Model = glm::translate(glm::vec3(0.0f));
	matrices.Projection = glm::perspective(45.0f, aspect,  0.01f, 10000.0f);
}
glm::vec3 glModel::projectPoint(const glm::vec3 &point,glm::mat3 &rotMatr,glm::vec3 &translateVector) {
	glm::vec3 rotVec = rotMatr * point;
	return rotVec+glm::vec3(translateVector);
}
void glModel::calculateSurfsWithoutBreaks() {
	// clear buffers

	vector<GLuint> indices; // indices buffer
	slicePath.clear();
	normalBuf.clear();
	drawPath.resize(0);
	sidesVerteces.resize(0);
	sidesNormals.resize(0);
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
		translation = userPath[n];
		if(n == 0) {
			T0 = glm::normalize(glm::vec3(userPath[1].x - userPath[0].x , userPath[1].y - userPath[0].y, userPath[1].z - userPath[0].z));
			N0 = glm::normalize(glm::cross(T0,glm::vec3(0.0f,1.0f,0.0f)));
			B0 = glm::normalize(glm::cross(T0,N0));
			T1 = T0;
			N1 = N0;
			B1 = B0;
		}
		else if(n != (pathSteps-1)) {
			T1 = userPath[n+1] - userPath[n];
			T1 = glm::normalize(T1);
			A =  -glm::cross(T0,T1)/ (glm::abs(glm::length(T0))*glm::abs(glm::length(T1)));
			if(glm::length(A) > 1e-4) {
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
				N0=N1;
				B0=B1;
			}
			T0=T1;

		}
		float a = n/(float (pathSteps))*0.0f;
		N1 = glm::rotate(N0, a, T1);
		B1 = glm::rotate(B0, a, T1);
		rotation = glm::mat3(T1, N1, B1);
		FOR(i,sliceVertCount) {
			v1 = projectPoint(glm::vec3(0, slice[i].y, slice[i].x), rotation, translation);
			slicePath.push_back(v1);
			GLint curVert = n * sliceVertCount + i;
			// calculate draw way
			if(n != pathSteps - 1) {
				GLint begSlice = n*sliceVertCount;
				GLint begNextSlice = (n + 1) * sliceVertCount;
				GLint endSlice = begNextSlice - 1;
				GLint endNextSlice = (n + 2) * sliceVertCount - 1;

				indices.push_back(curVert); // current vertex
				indices.push_back((curVert + sliceVertCount + 1) > endNextSlice ? begNextSlice : (curVert + sliceVertCount + 1)); // diagonal next slice vertex
				indices.push_back(curVert + sliceVertCount); // next slice direct vertex
				indices.push_back(curVert); // current vertex
				indices.push_back((curVert+1) > endSlice ? begSlice : (curVert + 1)); // next vertex
				indices.push_back((curVert + sliceVertCount + 1) > endNextSlice ? begNextSlice : (curVert + sliceVertCount + 1)); // diagonal next slice vertex
			}

		}
	}
	vector<glm::vec3>::iterator rangeBeg, rangeEnd;
	rangeEnd = slicePath.end();
	rangeBeg = rangeEnd - sliceVertCount;
	sidesVerteces.insert(sidesVerteces.begin(),rangeBeg,rangeEnd);
	GLuint indStart = 0;
	nrm = -calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	sidesNormals.insert(sidesNormals.end(),sliceVertCount,nrm);
	rangeBeg = slicePath.begin();
	rangeEnd = rangeBeg + sliceVertCount;
	sidesVerteces.insert(sidesVerteces.end(),rangeBeg,rangeEnd);
	indStart = sliceVertCount;
	nrm = calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	sidesNormals.insert(sidesNormals.end(),sliceVertCount,nrm);
	GLuint VCount = indices.size();
	for(GLuint i = 0; i < VCount; i++) {
		drawPath.push_back(slicePath[indices[i]]);
	}
	indices.clear();
	calculatingNormals(drawPath,normalBuf);
}
void glModel::calculateSurfsWithBreaks() {
	// clear buffers
	vector<GLuint> indices; // indices buffer
	slicePath.clear();
	normalBuf.clear();
	drawPath.clear();
	sidesVerteces.resize(0);
	sidesNormals.resize(0);
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

	FOR(n,brokenPathSteps)
	{
		translation = calcPath[n];
		if(n == 0) {
			T0 = glm::normalize(glm::vec3(userPath[1].x - userPath[0].x , userPath[1].y - userPath[0].y, userPath[1].z - userPath[0].z));
			N0 = glm::normalize(glm::cross(T0,glm::vec3(0.0f,1.0f,0.0f)));
			B0 = glm::normalize(glm::cross(T0,N0));
			T1 = T0;
			N1 = N0;
			B1 = B0;
		}
		else if(n != (brokenPathSteps-1)) {
			T1 = userPath[breaksMap[n]+1]-userPath[breaksMap[n]];
			T1 = glm::normalize(T1);
			A =  -glm::cross(T0,T1)/ (glm::abs(glm::length(T0))*glm::abs(glm::length(T1)));
			if(glm::length(A)>1e-5) {
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
				N0=N1;
				B0=B1;
			}
			T0=T1;
		}
		float a = GLfloat(n)/(GLfloat(brokenPathSteps))*rotAngles[n];
		N1 = glm::rotate(N0, a, T1);
		B1 = glm::rotate(B0, a, T1);
		rotation = glm::mat3(T1, N1, B1);
		FOR(i,sliceVertCount) {
			v1 = projectPoint(glm::vec3(0, slice[i].y, slice[i].x), rotation, translation);
			slicePath.push_back(v1);
			GLint curVert = n * sliceVertCount + i;
			// calculate draw way
			if(n != brokenPathSteps - 1) {
				GLint begSlice = n*sliceVertCount;
				GLint begNextSlice = (n + 1) * sliceVertCount;
				GLint endSlice = begNextSlice - 1;
				GLint endNextSlice = (n + 2) * sliceVertCount - 1;

				indices.push_back(curVert); // current vertex
				indices.push_back((curVert + sliceVertCount + 1) > endNextSlice ? begNextSlice : (curVert + sliceVertCount + 1)); // diagonal next slice vertex
				indices.push_back(curVert + sliceVertCount); // next slice direct vertex
				indices.push_back(curVert); // current vertex
				indices.push_back((curVert+1) > endSlice ? begSlice : (curVert + 1)); // next vertex
				indices.push_back((curVert + sliceVertCount + 1) > endNextSlice ? begNextSlice : (curVert + sliceVertCount + 1)); // diagonal next slice vertex
			}

		}
	}
	vector<glm::vec3>::iterator rangeBeg, rangeEnd;
	rangeEnd = slicePath.end();
	rangeBeg = rangeEnd - sliceVertCount;
	sidesVerteces.insert(sidesVerteces.begin(),rangeBeg,rangeEnd);
	GLuint indStart = 0;
	nrm = -calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	sidesNormals.insert(sidesNormals.end(),sliceVertCount,nrm);
	rangeBeg = slicePath.begin();
	rangeEnd = rangeBeg + sliceVertCount;
	sidesVerteces.insert(sidesVerteces.end(),rangeBeg,rangeEnd);
	indStart = sliceVertCount;
	nrm = calculateNormal(sidesVerteces[indStart],sidesVerteces[indStart+1],sidesVerteces[indStart+2]);
	sidesNormals.insert(sidesNormals.end(),sliceVertCount,nrm);
	GLuint VCount = indices.size();
	for(GLuint i = 0; i < VCount; i++) {
		drawPath.push_back(slicePath[indices[i]]);
	}
	indices.clear();
	calculatingNormals(drawPath,normalBuf);
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
void glModel::calculatingNormals(vector<glm::vec3> &vertecies, vector<glm::vec3> &normals) {
	normals.clear();
	glm::vec3 v1,v2,v3, nrm;
	for(GLuint i = 0; i < vertecies.size(); i+=3) {
		v1 = vertecies[i];
		v2 = vertecies[i+1];
		v3 = vertecies[i+2];
		nrm = calculateNormal(v1,v3,v2);
		normals.insert(normals.end(),3,nrm);
	}

}
void glModel::smoothingNormals(vector<glm::vec3> &nrmbuf) {
	glm::vec3 nrm1,nrm2,nrm3,nrm4;
	const GLuint vertsPerFace = 6U;
	GLuint vertsPerSegment = vertsPerFace * sliceVertCount;
	if(vertexCount != 0) {
		GLint nrmBufSz = (GLint) nrmbuf.size();
		if(modelStates.smoothNormals) {
			int ind_vert3 = -1, ind_vert4 = -1, ind_add1 = -1, ind_add2 = -1;
			for(int i = 0; i < vertexCount - vertsPerSegment; i += vertsPerFace) {
				ind_add1 = i + 5;
				ind_add2 = i + vertsPerSegment;
				ind_vert3 = ind_add2 + 3;
				ind_vert4 = ind_vert3 + 1;
				nrm1 = nrmbuf[i + 1];
				nrm2 = nrmbuf[i + 2];
				nrm3 = nrmbuf[ind_vert3];
				nrm4 = nrmbuf[ind_vert4];
				nrmbuf[ind_add1] = nrmbuf[i + 1] = nrmbuf[ind_vert4] = glm::normalize(nrm1 + nrm4);
				nrmbuf[ind_add2] = nrmbuf[i + 2] = nrmbuf[ind_vert3] = glm::normalize(nrm2 + nrm3);
			}
		}
	}
}
void glModel::moveCameraByKB(int Keys) {
	float Speed = 5.0f;

	if(Keys & 0x40) Speed *= 2.0f;
	if(Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * 0.05f;

	glm::vec3 Forward = Z, Right = X, Up = Y;

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
bool glModel::loadDialog(LPCWSTR dialogTitle, LPWSTR filename, LPCWSTR filter) {
	static WCHAR cwd[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,cwd);
	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = handle;
	ofn.lpstrFilter = filter;
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

	WCHAR buf[FILENAME_MAX];
	FILE *fp = NULL;
	userPath.clear();
	modelStates.loadPath = 0;
	modelStates.loadBreaks = 0;
	buf[0] = L'\0';
	if(modelStates.loadSlice) {
		if(loadDialog(L"Выбор файла пути",buf,L"Object Data Files\0*.OBJ\0\0")) {
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
					float segLen = 0.0f;
					pathSteps = userPath.size();
					totalPathLength = 0.0f;
					pathLengthes.clear();
					for(int i = 0; i < pathSteps - 1; i++) {
						segLen = glm::length(userPath[i]-userPath[i+1]);
						pathLengthes.push_back(segLen);
						totalPathLength += segLen;
					}
					modelStates.loadPath = 1;
					closeFile(fp);
				}
			}
		} 
	}
	else {
		MessageBox(handle,L"Сначала нужно загрузить сечение!",L"ICG GL Lab-2", MB_OK | MB_ICONINFORMATION);
	}
	if(modelStates.loadPath && modelStates.loadSlice) {
		calculateSurfsWithoutBreaks();
		TexturePath();
	}
}
void glModel::loadSliceData() {
	GLfloat min;
	GLint num = -1, k = 0;
	glm::vec2 vert;
	WCHAR buf[FILENAME_MAX];
	buf[0] = L'\0';
	FILE *fp;
	slice.clear();
	modelStates.loadSlice = 0;
	if(loadDialog(L"Выбор файла сечения",buf,L"Object Data Files\0*.OBJ\0\0")) {
		if(lstrlenW(buf) > 5) {
			fp = _wfopen(buf,L"r");
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
}
void glModel::loadPositionsData() {
	pathBreaks.clear();
	rotAngles.clear();
	WCHAR buf[FILENAME_MAX];
	buf[0] = L'\0';
	FILE *fp;
	modelStates.loadBreaks = 0;
	if(modelStates.loadSlice && modelStates.loadPath) {
		if(loadDialog(L"Загрузка позиций разбиения",buf,L"Plain-text files\0*.TXT\0\0")) {
			if(fp = _wfopen(buf,L"r")) {
				float pathBr, rotAng;
				while(!feof(fp)) {
					fgetws(buf,FILENAME_MAX,fp);
					swscanf(buf,L"%f %f",&pathBr,&rotAng);
					pathBreaks.push_back(pathBr);
					rotAngles.push_back(rotAng);
				}

			}
			float segLen, dist;

			brokenPathSteps = pathBreaks.size();
			glm::vec3 newSegStart, newSegEnd;
			for(int i = 0; i < brokenPathSteps; i++) {
				segLen = pathBreaks[i] * totalPathLength / 100.0f;
				dist = 0.0f;
				float prevDist = 0.0f;
				int k = 0;
				for(int j = 0; j < pathSteps; j++) {
					prevDist  = dist;
					dist += pathLengthes[j];

					if(dist >= segLen) {
						k = j;
						newSegStart = userPath[k];
						float lastSegLen = segLen - prevDist;
						newSegEnd = userPath[k+1]-newSegStart;
						newSegEnd = newSegStart + (newSegEnd * (lastSegLen / glm::length(newSegEnd)));
						break;
					}
				}
				breaksMap.push_back(k);
				calcPath.push_back(newSegEnd);
			}
			totalBrokenPathLength  = 0.0f;
			pathLengthes.clear();
			segLen = 0.0f;
			for(int i = 0; i < calcPath.size() - 1; i++) {
				segLen = glm::length(calcPath[i+1]-calcPath[i]);
				pathLengthes.push_back(segLen);
				totalBrokenPathLength += segLen;
			}
			modelStates.loadBreaks = 1;
		}
	}
	else {
		MessageBox(handle,L"Сначала нужно загрузить сечение и путь!",L"ICG GL Lab-2", MB_OK | MB_ICONEXCLAMATION);
	}
	if(modelStates.loadBreaks && modelStates.loadPath && modelStates.loadSlice) {
		calculateSurfsWithBreaks();
		TextureBPPath();
	}
}