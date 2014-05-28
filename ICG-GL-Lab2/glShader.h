#ifndef GLSHADER_H
#define GLSHADER_H
#include "glHeaders.h"
#include <vector>
#include <map>
#include <cstdarg>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLSL_VERTEX GL_VERTEX_SHADER
#define GLSL_FRAGMENT GL_FRAGMENT_SHADER
#define GLSL_GEORMETRY GL_GEOMETRY_SHADER
#define GLSL_TESSCONTROL GL_TESS_CONTROL_SHADER
#define GLSL_TESSEVAL GL_TESS_EVALUATION_SHADER
#define GLSL_COMPUTE GL_COMPUTE_SHADER
#define GLSL_NONE GL_ZERO
using std::vector;
using std::string;
using std::map;
using std::pair;
// setting variable error flags
#define SV_DATA_TYPE_MISMATCH_ERROR 0x0001
#define SV_NO_ERROR 0x0000
#define SV_UNKWOWN_ERROR 0xFFFF
#define SV_NAME_MISMATCH 0x0020
#define SV_POSITION_NEGATIVE_ERROR 0x0030
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
	map<string,GLint> uniformsLocs, attributesLocs;
	map<string,GLenum> uniformsTypes, attributesTypes;
	void printShaderLog(GLuint &shader);

	void infoMessage(LPCWSTR text);
	GLuint LoadShader(LPCWSTR path, GLenum type);
	char *loadFromFile(LPCWSTR path);
	bool attachShader(GLuint &shID);
public:
	bool checkAttrib(const GLchar *attrName) const;
	GLint getActiveAttrib(const GLchar *attrName) const;
	bool checkUniform(const GLchar *unifName) const;
	GLint getActiveUniform(const GLchar *unifName) const;
	
	bool validate() const;
	
	GLint setUniform1fv(const GLchar *unifName,const GLfloat *v) const;
	GLint setUniform2fv(const GLchar *unifName,const glm::vec2 &v) const;
	GLint setUniform3fv(const GLchar *unifName,const glm::vec3 &v) const;
	GLint setUniform4fv(const GLchar *unifName,const glm::vec4 &v) const;

	GLint setUniform1dv(const GLchar *unifName, const GLdouble *v) const;
	GLint setUniform2dv(const GLchar *unifName, const glm::dvec2 &v) const;
	GLint setUniform3dv(const GLchar *unifName, const glm::dvec3 &v) const;
	GLint setUniform4dv(const GLchar *unifName, const glm::dvec4 &v) const;

	GLint setUniform1iv(const GLchar *unifName,const GLint *v) const;
	GLint setUniform2iv(const GLchar *unifName,const glm::ivec2 &v) const;
	GLint setUniform3iv(const GLchar *unifName,const glm::ivec3 &v) const;
	GLint setUniform4iv(const GLchar *unifName,const glm::ivec4 &v) const;

	GLint setUniform1uiv(const GLchar *unifName,const GLuint *v) const;
	GLint setUniform2uiv(const GLchar *unifName,const glm::uvec2 &v) const;
	GLint setUniform3uiv(const GLchar *unifName,const glm::uvec3 &v) const;
	GLint setUniform4uiv(const GLchar *unifName,const glm::uvec4 &v) const; 

	GLint setUniform1f(const GLchar *unifName,const GLfloat v) const;
	GLint setUniform2f(const GLchar *unifName,const GLfloat v1, const GLfloat v2) const;
	GLint setUniform3f(const GLchar *unifName,const GLfloat v1, const GLfloat v2, const GLfloat v3) const;
	GLint setUniform4f(const GLchar *unifName,const GLfloat v1, const GLfloat v2, const GLfloat v3, const GLfloat v4) const;

	GLint setUniform1d(const GLchar *unifName, const GLdouble v) const;
	GLint setUniform2d(const GLchar *unifName, const GLdouble v1, const GLdouble v2) const;
	GLint setUniform3d(const GLchar *unifName, const GLdouble v1, const GLdouble v2, const GLdouble v3) const;
	GLint setUniform4d(const GLchar *unifName, const GLdouble v1, const GLdouble v2, const GLdouble v3, const GLdouble v4) const;

	GLint setUniform1i(const GLchar *unifName,const GLint v) const;
	GLint setUniform2i(const GLchar *unifName,const GLint v1,const GLint v2) const;
	GLint setUniform3i(const GLchar *unifName,const GLint v1,const GLint v2,const GLint v3) const;
	GLint setUniform4i(const GLchar *unifName,const GLint v1,const GLint v2,const GLint v3,const GLint v4) const;

	/*GLint setUniform1ui(const GLchar *unifName,const GLuint v) const;
	GLint setUniform2ui(const GLchar *unifName,const GLuint v1,const GLuint v2) const;
	GLint setUniform3ui(const GLchar *unifName,const GLuint v1,const GLuint v2,const GLuint v3) const;
	GLint setUniform4ui(const GLchar *unifName,const GLuint v1,const GLuint v2,const GLuint v3,const GLuint v4) const; */

	void printProgramLog();
	bool loadAndAttach(const shaderInfo *shaders);
	bool link(GLuint *getProgID);
	inline void createProgram() { programID = glCreateProgram(); }
	void BindAttribLocation(GLuint loc, const GLchar *attrName);
	const GLint getAttribLocation(const GLchar *attrName) const;
	const GLint getUniformLocation(const GLchar *unifName) const;
	void bindFragDataOutput(const GLchar *outName) const;
	GLboolean useProgram();
	void turnOff();
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
