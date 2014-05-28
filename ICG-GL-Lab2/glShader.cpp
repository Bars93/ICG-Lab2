#pragma warning(disable : 4996)
#include "glShader.h"
#include <cstdio>
#include <cstdarg>
#include <sstream>
#include <iterator>
using namespace std;
void glShader::infoMessage(LPCWSTR text) {
	MessageBox(handle,text,INFO_MSG_TITLE, MB_OK | MB_ICONEXCLAMATION);
}
void glShader::printProgramLog()
{
	//Make sure name is shader
	static WCHAR infoLogUnicode[1024];
	if( glIsProgram( programID ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &maxLength );
		vector<GLchar> infoLog(maxLength,'\0');
		//Allocate string
		LPSTR ilptr = &infoLog[0];

		//Get info log
		glGetProgramInfoLog( programID, maxLength, &infoLogLength, ilptr);
		if( infoLogLength > 0 )
		{
			//Print Log
			mbstowcs(infoLogUnicode,ilptr,min(maxLength,1024));
			infoMessage(infoLogUnicode);
		}
		infoLog.clear();
		//Deallocate string
	}
	else
	{
		wsprintf(infoLogUnicode,L"Object %u is not program",programID);
		infoMessage(infoLogUnicode);
	}
}
void glShader::BindAttribLocation(GLuint loc, const GLchar *attrName) {
	glBindAttribLocation(programID,loc,attrName);
}
const GLint glShader::getUniformLocation(const GLchar  *unifName) const {
	return  glGetUniformLocation(programID,(const GLchar *) unifName);
}
const GLint glShader::getAttribLocation(const GLchar *attrName) const {
	return glGetAttribLocation(programID,(const GLchar *) attrName);
}
void glShader::printShaderLog( GLuint &shader )
{
	//Make sure name is shader

	static WCHAR infoLogUnicode[512];
	if( glIsShader( shader ) )
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		vector<GLchar> infoLog(maxLength,'\0');
		LPSTR ilptr = &infoLog[0];
		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, ilptr);
		if( infoLogLength > 0 )
		{
			//Print Log
			swprintf(infoLogUnicode,L"%S",ilptr);
			infoMessage(infoLogUnicode);
		}
		infoLog.clear();
	}
	else
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		vector<GLchar> infoLog(maxLength,'\0');
		LPSTR ilptr = &infoLog[0];

		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, ilptr);
		if( infoLogLength > 0 )
		{
			if(maxLength > 0) {
				//Print Log
				wsprintf(infoLogUnicode,L"%S",ilptr);
				infoMessage(infoLogUnicode);
			}

		}
		infoLog.clear();
		wsprintf(infoLogUnicode,L"Object %u is not shader",shader);
		infoMessage(infoLogUnicode);
	}
}
glShader::glShader():
	programID(GL_ZERO),
	INFO_MSG_TITLE(L"GL SHADER CLASS"),
	pipeline(GL_ZERO),
	stagesSet(GL_ZERO),
	usePipeline(GL_ZERO)
{
	memset(&stages,0,sizeof(GLbitfield));
}
glShader::glShader(const shaderInfo *shaders) :
	programID(GL_ZERO),
	INFO_MSG_TITLE(L"GL SHADER CLASS"),
	pipeline(GL_ZERO),
	stagesSet(GL_ZERO),
	usePipeline(GL_ZERO)
{
	memset(&stages,0,sizeof(GLbitfield));
	loadAndAttach(shaders);
}
bool glShader::attachShader(GLuint &shID) {
	if(shID == NULL) {
		glDeleteProgram(programID);
		programID = NULL;
		return false;
	}
	glAttachObjectARB(programID,shID);
	GLenum error = OGLCheckError(errorString,L"attachShader",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}
GLuint glShader::LoadShader(LPCWSTR path, GLenum type) {
	using namespace std;
	GLuint shaderID;
	if(programID != NULL) {
		shaderID = glCreateShader(type);
		const GLchar* shaderSource = loadFromFile(path);
		GLint len = strlen(shaderSource);

		glShaderSource( shaderID, 1, (const GLchar**)&shaderSource,&len);
		//Compile shader source 
		glCompileShader( shaderID ); //Check shader for errors 

		GLint shaderCompiled = GL_FALSE; 
		glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled ); 
		GLint error = OGLCheckError(errorString,L"LoadShader",1024);
		if(error > 0) {
			MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
			return 0;
		}
		if( shaderCompiled != GL_TRUE ) { 
			wsprintf(str,L"Unable to compile shader %d!\n\nSource:\n%S\n", shaderID, shaderSource ); 
			infoMessage(str);
			printShaderLog( shaderID ); 
			glDeleteShader( shaderID ); 
			shaderID = NULL; 
		} 
		delete[] shaderSource;

	}
	else {
		shaderID = NULL;
	}
	return shaderID;
}
char* glShader::loadFromFile(LPCWSTR path) {
	const size_t blockSize = 512;
	FILE *fp;
	char buf[blockSize];
	char *source = NULL;
	size_t tmp, sourceLength = 0;

	/* open file */
	fp = _wfopen(path, L"r");
	if(!fp) {
		return NULL;
	}

	/* read the entire file into a string */
	while((tmp = fread(buf, 1, blockSize, fp)) > 0) {
		char *newSource = new char[sourceLength + tmp + 1];

		if(source) {
			memcpy(newSource, source, sourceLength);
			delete[] source;
		}
		memcpy(newSource + sourceLength, buf, tmp);

		source = newSource;
		sourceLength += tmp;
	}

	/* close the file and null terminate the string */
	fclose(fp);
	if(source)
		source[sourceLength] = '\0';
	return source;
}
void glShader::setProgramParameteri(GLenum name, GLint val) {
	if(name == GL_PROGRAM_SEPARABLE && val != GL_ZERO) {
		usePipeline = true;
		glGenProgramPipelines(1,&pipeline);
	}
	glProgramParameteri(programID,name,val);
}
void glShader::setPipelineStages(GLbitfield inStages) {
	if(usePipeline) {
		glUseProgramStages(pipeline,inStages,programID);
		stagesSet = true;
	}
}
GLboolean glShader::useProgram() {
	//Use shader
	GLint param;

	if(!stagesSet && usePipeline) {
		stages = 0;
		for(vector<GLuint>::iterator i = shaderList.begin(); i != shaderList.end(); i++) {
			if(glIsShader(*i)) {
				glGetShaderiv(*i,GL_SHADER_TYPE,&param);
				switch(param) {
				case GL_VERTEX_SHADER:
					stages |= GL_VERTEX_SHADER_BIT;
					break;
				case GL_FRAGMENT_SHADER:
					stages |= GL_FRAGMENT_SHADER_BIT;
					break;
				case GL_TESS_CONTROL_SHADER:
					stages |= GL_TESS_CONTROL_SHADER_BIT;
					break;
				case GL_TESS_EVALUATION_SHADER:
					stages |=  GL_TESS_EVALUATION_SHADER_BIT;
					break;
				case GL_COMPUTE_SHADER:
					stages |= GL_COMPUTE_SHADER_BIT;
					break;
				}
			}
			else {
				return false;
			}
		}
		glUseProgramStages(pipeline,stages,programID);
	}
	if(usePipeline && stagesSet) {
		glBindProgramPipeline(pipeline);
	}
	else {
		glUseProgram(programID);
	}
	//Check for error
	GLenum error = OGLCheckError(errorString,L"useProgram",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return 0;
	}
	if( error != GL_NO_ERROR )
	{
		wsprintf(str,L"Error binding shader! %S\n", gluErrorString( error ) );
		printProgramLog();
		return false;
	}
	return true;
}
void glShader::turnOff() {
	if(usePipeline)
		glBindProgramPipeline(GL_ZERO);
	else
		glUseProgram(GL_ZERO);

}
bool glShader::link(GLuint *getProgID) {

	glLinkProgram(programID);
	GLint programSuccess = GL_TRUE;
	glGetProgramiv( programID, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
	{
		printf( "Error linking program %d!\n", programID );
		printProgramLog();
		glDeleteProgram(programID);
		programID = NULL;
		return false;
	}
	else {
		GLint attrLen, attrNum, writtenSymbols,varSize, varLoc;
		GLenum varType;
		vector<GLchar> nameBuf;
		glGetProgramiv(programID,GL_ACTIVE_ATTRIBUTES,&attrNum);
		glGetProgramiv(programID,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&attrLen);
		attributesLocs.clear();
		attributesTypes.clear();
		for(GLuint i = 0; i < (GLuint) attrNum; i++) {
			nameBuf.resize(attrLen,'\0');
			glGetActiveAttrib(programID,i,attrLen,&writtenSymbols,&varSize,&varType,&nameBuf[0]);
			varLoc = glGetAttribLocation(programID,&nameBuf[0]);
			attributesLocs.insert(pair<string,GLint>(string(&nameBuf[0]),varLoc));
			attributesTypes.insert(pair<string,GLenum>(string(&nameBuf[0]),varType));
			nameBuf.clear();
		}
		nameBuf.clear();
		GLint unifLen, unifNum;
		glGetProgramiv(programID,GL_ACTIVE_UNIFORMS,&unifNum);
		glGetProgramiv(programID,GL_ACTIVE_UNIFORM_MAX_LENGTH,&unifLen);
		uniformsLocs.clear();
		uniformsTypes.clear();
		for(GLuint i = 0; i < (GLuint) unifNum; i++) {
			nameBuf.resize(unifLen,'\0');
			glGetActiveUniform(programID,i,unifLen,&writtenSymbols,&varSize,&varType,&nameBuf[0]);
			varLoc = glGetUniformLocation(programID,&nameBuf[0]);
			uniformsLocs.insert(pair<string,GLint>(string(&nameBuf[0]),varLoc));
			uniformsTypes.insert(pair<string,GLenum>(string(&nameBuf[0]),varType));
			nameBuf.clear();
		}
	}
	if(getProgID != NULL && programID != NULL)
		*getProgID = programID;
	return true;
}
bool glShader::loadAndAttach(const shaderInfo *shaders) {
	bool globRes = true;
	if(programID == NULL)
		programID = glCreateProgram(); // create GLSL program
	GLenum error = OGLCheckError(errorString,L"loadAndAttach",1024);
	if(error > 0) {
		MessageBox(handle,errorString,L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
		return 0;
	}
	GLuint currentShader;
	GLint i = 0;
	while(globRes && shaders[i].sType != GLSL_NONE) {
		currentShader = LoadShader(shaders[i].path,shaders[i].sType);
		if(!attachShader(currentShader)) 
			globRes = false;
		else 
			shaderList.push_back(currentShader);
		i++;
	}
	return globRes;
}
void glShader::bindFragDataOutput(const GLchar *outName) const {
	glBindFragDataLocation(programID,0,outName);
}
GLint glShader::setUniform1fv(const GLchar *unifName,const GLfloat *v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_FLOAT)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform1fv(foundLoc->second,1,v);
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform1f(const GLchar *unifName,const GLfloat v) const {
	return setUniform1fv(unifName,&v);
}
GLint glShader::setUniform2fv(const GLchar *unifName,const glm::vec2 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_FLOAT_VEC2)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform2fv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform2f(const GLchar *unifName,const GLfloat v1, const GLfloat v2) const {
	glm::vec2 v(v1,v2);
	return setUniform2fv(unifName,v);
}
GLint glShader::setUniform3fv(const GLchar *unifName,const glm::vec3 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_FLOAT_VEC3)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform3fv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform3f(const GLchar *unifName,const GLfloat v1, const GLfloat v2, const GLfloat v3) const {
	glm::vec3 v(v1,v2,v3);
	return setUniform3fv(unifName,v);
}
GLint glShader::setUniform4fv(const GLchar *unifName,const glm::vec4 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_FLOAT_VEC4)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform4fv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform4f(const GLchar *unifName,const GLfloat v1, const GLfloat v2, const GLfloat v3,const GLfloat v4) const {
	glm::vec4 v(v1,v2,v3,v4);
	return setUniform4fv(unifName,v);
}
GLint glShader::setUniform1dv(const GLchar *unifName,const GLdouble *v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_DOUBLE)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform1dv(foundLoc->second,1,v);
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform1d(const GLchar *unifName, const GLdouble v) const {
	return setUniform1dv(unifName,&v);
}
GLint glShader::setUniform2dv(const GLchar *unifName,const glm::dvec2 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_DOUBLE_VEC2)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform2dv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}

GLint glShader::setUniform2d(const GLchar *unifName, const GLdouble v1, const GLdouble v2) const {
	glm::dvec2 v(v1,v2);
	return setUniform2dv(unifName,v);
}
GLint glShader::setUniform3dv(const GLchar *unifName,const glm::dvec3 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_DOUBLE_VEC3)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform3dv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform3d(const GLchar *unifName, const GLdouble v1, const GLdouble v2,const GLdouble v3) const {
	glm::dvec3 v(v1,v2,v3);
	return setUniform3dv(unifName,v);
}
GLint glShader::setUniform4dv(const GLchar *unifName,const glm::dvec4 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_DOUBLE_VEC4)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform4dv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform4d(const GLchar *unifName, const GLdouble v1, const GLdouble v2,const GLdouble v3, const GLdouble v4) const {
	glm::dvec4 v(v1,v2,v3,v4);
	return setUniform4dv(unifName,v);
}
GLint glShader::setUniform1iv(const GLchar *unifName,const GLint *v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_INT && foundType->second != GL_BOOL)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform1iv(foundLoc->second,1,v);
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform1i(const GLchar *unifName,const GLint v) const {
	return setUniform1iv(unifName,&v);
}
GLint glShader::setUniform2iv(const GLchar *unifName,const glm::ivec2 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_INT_VEC2)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform2iv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;

}
GLint glShader::setUniform2i(const GLchar *unifName,const GLint v1,const GLint v2) const {
	glm::ivec2 v(v1,v2);
	return setUniform2iv(unifName,v);
}
GLint glShader::setUniform3iv(const GLchar *unifName,const glm::ivec3 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_INT_VEC3)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform3iv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;
}
GLint glShader::setUniform3i(const GLchar *unifName,const GLint v1,const GLint v2,const GLint v3) const {
	glm::ivec3 v(v1,v2,v3);
	return setUniform3iv(unifName,v);
}
GLint glShader::setUniform4iv(const GLchar *unifName,const glm::ivec4 &v) const {
	string uName(unifName);
	map<string,GLenum>::const_iterator foundType;
	map<string,GLint>::const_iterator foundLoc;
	foundType = uniformsTypes.find(uName);
	if(uniformsTypes.find(uName) == uniformsTypes.end()) {
		return SV_NAME_MISMATCH;
	}
	else {
		if(foundType->second != GL_INT_VEC4)
			return SV_DATA_TYPE_MISMATCH_ERROR;
		else {
			foundLoc = uniformsLocs.find(uName);
			if(foundLoc->second != -1)
				glUniform4iv(foundLoc->second,1,glm::value_ptr(v));
			else
				return SV_POSITION_NEGATIVE_ERROR;
		}
	}
	return SV_NO_ERROR;

}
GLint glShader::setUniform4i(const GLchar *unifName,const GLint v1,const GLint v2,const GLint v3,const GLint v4) const {
	glm::ivec4 v(v1,v2,v3,v4);
	return setUniform4iv(unifName,v);
}
glShader::~glShader(void)
{
	for(vector<GLuint>::reverse_iterator i = shaderList.rbegin(); i != shaderList.rend(); i++) {
		glDetachShader(programID,*i);
		glDeleteShader(*i);
	}
	shaderList.clear();
	glDeleteProgram(programID);
	glDeleteProgramPipelines(1,&pipeline);
}
