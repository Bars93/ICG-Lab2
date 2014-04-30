#ifndef GLMODEL_H
#define GLMODEL_H
#include "glHeaders.h"
#include "glShader.h"
#include <vector>
// GLM library using
#include <boost/static_assert.hpp>
#define GLM_FORCE_SSE2
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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
	USHORT lighting: 1;
	USHORT remake : 1;
	USHORT redraw : 1;
	USHORT loadPath : 1;
	USHORT loadSlice : 1;
	USHORT loadTex : 1;
	USHORT bindTex : 1;
};
class glModel
{
private:
	GLfloat width, height; // width and height of client window space
	HWND handle; // window handle
	GLfloat normalDirect;
	// mouse state
	GLfloat mouseX, mouseY;
	// camera properties
	glm::vec3 X, Y, Z, Position, Reference; // Camera's options
	// shader's variables
	glShader *GLSL; // shader's loader and attacher
	GLuint program; // shader program ID
	// buffer's variables
	GLuint vbID, vnID, vnVecBuf; // buffers ID: vbID - vertex buffer, vnID - vertexNormals, vnVecBuf - normal vector's
	GLuint vertexCount; // count of vertexes


	// buffer storages;
	vector<glm::vec3> quadBuf; // quads buffer (temporary)
	vector<glm::vec3> normalBuf, normalBuf2, normalBuf3, normalBuf4; // normal buffer
	//vector<glm::vec3> normalVecBuf; // normal vector's buffer

	//vector<glm::vec3> axisBuf; // axis vectors (depr)
	vector<glm::vec3> userPath; // user define path (original)
	vector<glm::vec3> slicePath; // calculated path
	vector<glm::vec2> slice; // slice
	GLint sliceVertCount, sliceSize, pathSteps;
	glm::mat4x4 modelMatr, projMatr, ViewMatrix; // matrices
	// core
	bitStates modelStates;

	bool initBuffers();
	void initLights();

	bool LoadTexture(LPCWSTR filename);
	bool bindTexture();
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

	void setMouseLeft(bool flag) { modelStates.mouseLeftDown = flag; };
	void setMouseRight(bool flag) { modelStates.mouseRightDown = flag; };
	void setMousePosition(int x, int y) { mouseX = GLfloat(x); mouseY = GLfloat(y); };
	void changeLight() { modelStates.lighting = modelStates.lighting == 1 ? 0 : 1;};
	void rotateCamera(int x, int y);
	void zoomCamera(int dist);
	glModel(void);
	~glModel(void);
	// set functions
	void setWinHandle(HWND _handle) { handle = _handle; }
};
#endif
