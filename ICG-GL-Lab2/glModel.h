#ifndef GLMODEL_H
#define GLMODEL_H
#include "glHeaders.h"
#include "glShader.h"
#include <vector>
// GLM library using
#include <boost/static_assert.hpp>
#define GLM_FORCE_SSE2
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/rotate_vector.hpp>
using std::vector;
class glModel
{
	GLfloat width, height; // width and height of client window space
	HWND handle; // window handle
	// mouse state
	GLfloat mouseX, mouseY;
	bool mouseLeftDown;
	bool mouseRightDown;
	// camera properties
	glm::vec3 X, Y, Z, Position, Reference; // Camera's options
	// shader's variables
	BOOL useShadersflg; // using shader's flag 
	glShader *GLSL; // shader's loader and attacher
	GLuint program; // shader program ID
	// buffer's variables
	GLuint vbID, vnID, vnVecBuf; // buffers ID: vbID - vertex buffer, vnID - vertexNormals, vnVecBuf - normal vector's
	GLuint vertexCount; // count of vertexes
	// attibute and uniform variables
	GLint colAttrib,attrLoc; // attribute location - may be deprecated
	// buffer storages;
	vector<glm::vec3> quadBuf; // quads buffer (temporary)
	vector<glm::vec3> normalBuf; // normal buffer
	vector<glm::vec3> normalVecBuf; // normal vector's buffer
	// matrices
	GLdouble matr[16]; // matrix storage
	glm::mat4x4 *viewMatr, *viewMatrInverse, modelMatr, projMatr, ViewMatrix; // matrices
	// core
	struct {
		float *inBuf;
		float *outBuf;
	
	} vertData;
	bool remake;
public:
	bool init();    // initialize OpenGL states
	bool initBuffers();

	void moveCameraByKB(int key);
	void setCamera(GLdouble posX, GLdouble posY, GLdouble posZ, GLdouble targetX, GLdouble targetY, GLdouble targetZ);
	void calculateViewMatrix();
	void setViewport(int width, int height);
	void setMatrPtrs(glm::mat4x4 *_viewMatr, glm::mat4x4 *_viewMatrInv);
	void lookCamera(const glm::vec3 &pos, const glm::vec3 &Ref, bool RotateAroundReference);
	void draw();

	void setMouseLeft(bool flag) { mouseLeftDown = flag; };
	void setMouseRight(bool flag) { mouseRightDown = flag; };
	void setMousePosition(int x, int y) { mouseX = GLfloat(x); mouseY = GLfloat(y); };

	void rotateCamera(int x, int y);
	void zoomCamera(int dist);
	glModel(void);
	~glModel(void);
	// set functions
	void setWinHandle(HWND _handle) { handle = _handle; }
};
#endif
