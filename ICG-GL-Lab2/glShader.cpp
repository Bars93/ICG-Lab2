#pragma warning(disable : 4996)
#include "glShader.h"
#include <stdio.h>
#include <string.h>
void glShader::infoMessage(LPCWSTR text) {
	MessageBox(handle,text,INFO_MSG_TITLE, MB_OK | MB_ICONEXCLAMATION);
}
void glShader::printProgramLog( GLuint program )
{
	//Make sure name is shader
	static WCHAR infoLogUnicode[512];
	static CHAR infoLog[512];
	if( glIsProgram( program ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		LPCSTR ilptr = infoLog;

		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			mbstowcs(infoLogUnicode,ilptr,maxLength);
			infoMessage(infoLogUnicode);
		}
		//Deallocate string
	}
	else
	{
		wsprintf(infoLogUnicode,L"Object %u is not program",program);
		infoMessage(infoLogUnicode);

	}
}
void glShader::printShaderLog( GLuint shader )
{
	//Make sure name is shader
	
	static WCHAR infoLogUnicode[512];
	static CHAR infoLog[512];
	if( glIsShader( shader ) )
	{
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
		LPCSTR ilptr = infoLog;

		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			//Print Log
			wsprintf(infoLogUnicode,L"%S",infoLog);
			infoMessage(infoLogUnicode);
		}
	}
	else
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		LPSTR infoLog = new CHAR[ maxLength ];
		LPCSTR ilptr = infoLog;

		//Get info log
		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			if(maxLength > 0) {
				//Print Log
				wsprintf(infoLogUnicode,L"%S",infoLog);
				infoMessage(infoLogUnicode);
			}

		}
		wsprintf(infoLogUnicode,L"Object %u is not shader",shader);
		infoMessage(infoLogUnicode);
	}
}
glShader::glShader(HWND _handle):
	handle(_handle),
	INFO_MSG_TITLE(L"GL SHADER CLASS")
{
	str = new WCHAR[256];
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(handle,L"GLEW is not initialized!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
	}
	shader_vertexID = shader_fragmentID = programID = NULL;
}
glShader::glShader(LPCSTR path_vert, LPCSTR path_frag, HWND _handle) :
	handle(_handle),
	INFO_MSG_TITLE(L"GL SHADER CLASS")
{
	str = new WCHAR[256];
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		MessageBox(handle,L"GLEW is not initialized!",L"ICG GL Lab-2", MB_OK | MB_ICONERROR);
	}
	loadAndAttach(path_vert,path_frag);
}
bool glShader::attachShader(GLuint shID) {
	if(shID == NULL) {
		glDeleteProgram(programID);
		programID = NULL;
		return false;
	}
	glAttachObjectARB(programID,shID);
	return true;
}
GLuint glShader::LoadShader(LPCSTR path, UINT32 type) {
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
		PFNGLGETSHADERIVPROC glGetShaderivOwn = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
		glGetShaderivOwn( shaderID, GL_COMPILE_STATUS, &shaderCompiled ); 
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
char* glShader::loadFromFile(LPCSTR path) {
	const size_t blockSize = 512;
	FILE *fp;
	char buf[blockSize];
	char *source = NULL;
	size_t tmp, sourceLength = 0;

	/* open file */
	fp = fopen(path, "r");
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
GLboolean glShader::useProgram() {
	//Use shader
	glUseProgram(programID);

	//Check for error
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
	{
		wsprintf(str,L"Error binding shader! %S\n", gluErrorString( error ) );
		printProgramLog(programID);
		return false;
	}
	return true;
}
GLuint glShader::loadAndAttach(LPCSTR path_vert, LPCSTR path_frag) {
	if(programID == NULL)
		programID = glCreateProgramObjectARB(); // create GLSL program
	shader_vertexID = LoadShader(path_vert,GLSL_VERTEX);
	if(attachShader(shader_vertexID)) {
		shader_fragmentID = LoadShader(path_frag,GLSL_FRAGMENT);
		if(attachShader(shader_fragmentID)) {
			//Check for errors
			glLinkProgram(programID);
			GLint programSuccess = GL_TRUE;
			glGetProgramiv( programID, GL_LINK_STATUS, &programSuccess );
			if( programSuccess != GL_TRUE )
			{
				printf( "Error linking program %d!\n", programID );
				printProgramLog( programID );
				glDeleteProgram( programID );
				programID = NULL;
				return -1;
			}
			return programID;
		}
	}
	return -1;
}
glShader::~glShader(void)
{
	delete[] str;
	glDeleteShader(shader_vertexID);
	glDeleteShader(shader_fragmentID);
	glDeleteProgram(programID);
}
