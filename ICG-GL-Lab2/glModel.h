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
struct bitStates {
	USHORT mouseLeftDown : 1;
	USHORT mouseRightDown : 1;
	USHORT useShadersflg : 1;
	USHORT axisDraw : 1;
	USHORT remake : 1;
	USHORT redraw : 1;
};
class glModel
{
private:
	GLfloat width, height; // width and height of client window space
	HWND handle; // window handle
	GLfloat normalDirect;
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


	// buffer storages;
	vector<glm::vec3> quadBuf; // quads buffer (temporary)
	vector<glm::vec3> normalBuf, normalBuf2, normalBuf3, normalBuf4; // normal buffer
	vector<glm::vec3> normalVecBuf; // normal vector's buffer

	vector<glm::vec3> axisBuf;
	vector<glm::vec3> userPath;
	vector<glm::vec3> slices;
	// matrices
	GLdouble matr[16]; // matrix storage
	glm::mat4x4 *viewMatr, *viewMatrInverse, modelMatr, projMatr, ViewMatrix; // matrices
	// core
	bool remake;
	bitStates modelStates;

	bool initBuffers();
	bool initLights();
	bool initTextures();

	bool loadDialog(LPCWSTR dialogTitle, LPWSTR filename);

	glm::vec3 calculateNormal(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c);
public:
	bool init();    // initialize OpenGL states

	void loadFileData();
	void moveCameraByKB(int key);
	//void setCamera(GLdouble posX, GLdouble posY, GLdouble posZ, GLdouble targetX, GLdouble targetY, GLdouble targetZ);
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
