#pragma warning(disable : 4996)
#include "glShader.h"
#include <stdio.h>
#include <string.h>
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
			wsprintf(infoLogUnicode,L"%S",ilptr);
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
bool glShader::link(GLuint *getProgID) {
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
	if(getProgID != NULL)
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
