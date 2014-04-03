#ifndef GLSHADER_H
#define GLSHADER_H
#include "glHeaders.h"

#define GLSL_VERTEX GL_VERTEX_SHADER
#define GLSL_FRAGMENT GL_FRAGMENT_SHADER

class glShader
{
	WCHAR *str;
	LPCWSTR INFO_MSG_TITLE;
	HWND handle;
	GLuint shader_vertexID, shader_fragmentID, programID;

	void printProgramLog(GLuint Program);
	void printShaderLog(GLuint shader);
	void infoMessage(LPCWSTR text);
	GLuint LoadShader(LPCSTR path, UINT32 type);
	bool attachShader(GLuint shID);
public:
	bool loadAndAttach(LPCSTR path_vert, LPCSTR path_frag);
	inline void createProgram() { programID = glCreateProgram(); }
	GLboolean useProgram();
	
	glShader(HWND _handle);
	glShader(LPCSTR path_vert, LPCSTR path_frag, HWND _handle);
	~glShader(void);
	/* get functions */
	GLuint getProgramID() const { return programID; } 
	/* bind/scope data functions */
	// TODO:
};
#endif
