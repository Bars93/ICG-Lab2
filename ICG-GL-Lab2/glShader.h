#ifndef GLSHADER_H
#define GLSHADER_H
#include "glHeaders.h"
#include <vector>
#define GLSL_VERTEX GL_VERTEX_SHADER
#define GLSL_FRAGMENT GL_FRAGMENT_SHADER
#define GLSL_GEORMETRY GL_GEOMETRY_SHADER
#define GLSL_TESSCONTROL GL_TESS_CONTROL_SHADER
#define GLSL_TESSEVAL GL_TESS_EVALUATION_SHADER
#define GLSL_COMPUTE GL_COMPUTE_SHADER
#define GLSL_NONE GL_ZERO
using std::vector;
typedef struct tagShaderInfo {
	LPCWSTR path;
	GLenum sType;
} shaderInfo;
class glShader
{
	WCHAR str[256], errorString[1024];
	LPCWSTR INFO_MSG_TITLE;
	HWND handle;
	GLuint programID;
	vector<GLuint> shaderList;
	GLuint pipeline;
	GLbitfield stages;
	bool stagesSet, usePipeline;
	void printProgramLog();
	void printShaderLog(GLuint &shader);

	void infoMessage(LPCWSTR text);
	GLuint LoadShader(LPCWSTR path, GLenum type);
	char *loadFromFile(LPCWSTR path);
	bool attachShader(GLuint &shID);
public:
	bool loadAndAttach(const shaderInfo *shaders);
	bool link(GLuint *getProgID);
	inline void createProgram() { programID = glCreateProgram(); }
	void BindAttribLocation(GLuint loc, const GLchar *attrName);
	const GLint getAttribLocation(const GLchar *attrName) const;
	const GLint getUniformLocation(const GLchar *unifName) const;
	void bindFragDataOutput(const GLchar *outName) const;
	GLboolean useProgram();

	glShader();
	glShader(const shaderInfo *shaders);
	~glShader(void);
	/* get functions */
	void setProgramParameteri(GLenum name, GLint val);
	void setPipelineStages(GLbitfield inStages);
	GLuint getProgramID() const { return programID; } 
	GLuint getPipeline() const { return pipeline; }
	void setWinHandle(HWND *hwnd) { handle = *hwnd; }
	/* bind/scope data functions */
	// TODO:
};
#endif
