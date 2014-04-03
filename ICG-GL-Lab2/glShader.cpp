#pragma warning(disable : 4996)
#include "glShader.h"
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <string>
#define sizeArray(x) sizeof(x)/sizeof(x[0])
using std::string;
void glShader::infoMessage(LPCWSTR text) {
	MessageBox(handle,text,INFO_MSG_TITLE, MB_OK | MB_ICONEXCLAMATION);
}
void glShader::printProgramLog( GLuint program )
{
	//Make sure name is shader
	LPWSTR infoLogUnicode;
	if( glIsProgram( program ) )
	{
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

		//Allocate string
		LPSTR infoLog = new CHAR[ maxLength ];
		LPCSTR ilptr = infoLog;

		//Get info log
		glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
		if( infoLogLength > 0 )
		{
			infoLogUnicode = new WCHAR[ maxLength ];
			//Print Log
			mbstowcs(infoLogUnicode,ilptr,maxLength);
			infoMessage(infoLogUnicode);
		}
		//Deallocate string
	}
	else
	{
		infoLogUnicode = new WCHAR[256];
		wsprintf(infoLogUnicode,L"Object %u is not program",program);
		infoMessage(infoLogUnicode);

	}
	delete[] infoLogUnicode;
}
void glShader::printShaderLog( GLuint shader )
{
	//Make sure name is shader
	LPWSTR infoLogUnicode;
	if( glIsShader( shader ) )
	{
		//Shader log length
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
			infoLogUnicode = new WCHAR[ maxLength ];
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
				infoLogUnicode = new WCHAR[ maxLength ];
				//Print Log
				wsprintf(infoLogUnicode,L"%S",infoLog);
				infoMessage(infoLogUnicode);
			}
			else {
				infoLogUnicode = NULL;	
			}
		}
		if(!infoLogUnicode) 
			infoLogUnicode = new WCHAR[256];
		wsprintf(infoLogUnicode,L"Object %u is not shader",shader);
		infoMessage(infoLogUnicode);
	}
	delete[] infoLogUnicode;
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
		string shaderContent; // file content of shader;
		shaderID = 0; // setting defaults
		ifstream shaderFS(path, ios::in); // open input file stream
		if(shaderFS.is_open()) {
			// read until met EOF
			shaderContent.assign(istreambuf_iterator<char>(shaderFS),istreambuf_iterator<char>());
			shaderFS.close();
			glCreateShaderObjectARB(type);
			const GLchar* shaderSource = shaderContent.c_str();
			GLuint len = shaderContent.length();

			glShaderSourceARB( shaderID, 1, (const GLchar**)&shaderSource,NULL);
			//Compile shader source 
			glCompileShaderARB( shaderID ); //Check shader for errors 
			GLint shaderCompiled = GL_FALSE; 
			glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled ); 
			if( shaderCompiled != GL_TRUE ) { 
				wsprintf(str,L"Unable to compile shader %d!\n\nSource:\n%S\n", shaderID, shaderSource ); 
				infoMessage(str);
				printShaderLog( shaderID ); 
				glDeleteShader( shaderID ); 
				shaderID = NULL; 
			} 
		}
		else { 
			wsprintf(str,L"Unable to open file %S\n", path); 
			infoMessage(str);
		} 
	}
	else {
		shaderID = NULL;
	}
	return shaderID;
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
bool glShader::loadAndAttach(LPCSTR path_vert, LPCSTR path_frag) {
	if(programID == NULL)
		programID = glCreateProgramObjectARB(); // create GLSL program
	shader_vertexID = LoadShader(path_vert,GLSL_VERTEX);
	if(attachShader(shader_vertexID)) {
		shader_fragmentID = LoadShader(path_frag,GLSL_FRAGMENT);
		if(attachShader(shader_fragmentID)) {
			//Check for errors
			glLinkProgramARB(programID);
			GLint programSuccess = GL_TRUE;
			glGetProgramiv( programID, GL_LINK_STATUS, &programSuccess );
			if( programSuccess != GL_TRUE )
			{
				printf( "Error linking program %d!\n", programID );
				printProgramLog( programID );
				glDeleteProgram( programID );
				programID = NULL;
				return false;
			}
			return true;
		}
	}
	return false;
}
glShader::~glShader(void)
{
	delete[] str;
	glDeleteShader(shader_vertexID);
	glDeleteShader(shader_fragmentID);
	glDeleteProgram(programID);
}
