#ifndef GLMODEL_H
#define GLMODEL_H
#include "glHeaders.h"
#include "glShader.h"
#include <vector>
// GLM library using
#include <boost/static_assert.hpp>
#define GLM_FORCE_SSE2
#define GLM_SWIZZLE
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
	USHORT useShadersflg : 1; // deprecated in next release
	USHORT shaderInUse : 1;
	USHORT lighting: 1;
	USHORT showNormals : 1;
	USHORT remake : 1;
	USHORT loadPath : 1;
	USHORT loadSlice : 1;
	USHORT bufferBind : 1;
	USHORT loadTex : 1;
	USHORT bindTex : 1;
	USHORT readyToDraw : 1;
};
class glModel
{
private:
	WCHAR errorString[1024];
	GLfloat width, height; // width and height of client window space
	HWND handle; // window handle
	HDC hDevC;
	GLfloat normalDirect;
	// mouse state
	GLfloat mouseX, mouseY;
	// camera properties
	glm::vec3 X, Y, Z, Position, Reference; // Camera's options
	// shader's variables
	glShader MainShader, MainWithLightShader, NormalVecShader;
	GLuint ProgLight, ProgDelight; // shader program ID
	// buffer's variables
	const enum vaoIDs { quadsVA, normVecVA, VAOcount };
	GLuint VAOs[VAOcount];	
	const enum bufferIDs {vbID, vnID, vnVecBufID, colorBufID, bufCount };
	const enum vertAttribs { vertPosition, vertColor, normalVec, attribCount };
	const enum uniformVars { MVMatr, MVPMatr, NormMatr, uniformCount };
	const enum lightsVecs { mLight0, lightsCount };
	glm::vec4 lightPos[lightsCount];
	glm::vec4 lightDifColor[lightsCount];
	glm::vec4 lightAmbientColor[lightsCount];
	glm::vec4 lightSpecColor[lightsCount];
	GLuint buffers[bufCount];
	GLuint shaderUniformLocs[uniformCount];
	GLuint shaderAttribLocs[attribCount];
	GLuint vertexCount; // count of vertexes

	// buffer storages;
	vector<glm::vec4> quadBuf; // quads buffer (temporary)
	vector<glm::vec3> normalBuf; // normal buffer
	vector<glm::vec3> colorBuf;
	vector<GLuint> indices;
	//vector<glm::vec3> normalVecBuf; // normal vector's buffer

	//vector<glm::vec3> axisBuf; // axis vectors (depr)
	vector<glm::vec3> userPath; // user define path (original)
	vector<glm::vec3> slicePath; // calculated path
	vector<glm::vec3> normVBuf;
	vector<glm::vec2> slice; // slice
	GLuint sliceVertCount, sliceSize, pathSteps, normVecCount;
	glm::mat4 modelMatr, projMatr, ViewMatrix, MV, MVP, normalMatrix; // matrices
	// core
	bitStates modelStates;

	void swapDevBuffers();
	
	void initLights();

	bool LoadTexture(LPCWSTR filename);
	bool bindTexture();
	bool initTextures();
	
	void RebindShaderAttributes();
	bool loadDialog(LPCWSTR dialogTitle, LPWSTR filename);
	
	void calcNormVectors();
	glm::vec3 calculateNormal(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c);
	void calculateViewMatrix();

	void initLightingMode();
	void initDelightingMode();
	void bindLightShaderVariables();
	void bindDelightShaderVariables();
	void unbindAllShaders();
public:
	bool initBuffers();
	bool init();    // initialize OpenGL states
	void checkReadyToDraw();
	void loadFileData();
	void moveCameraByKB(int key);
	//void setCamera(GLdouble posX, GLdouble posY, GLdouble posZ, GLdouble targetX, GLdouble targetY, GLdouble targetZ);
	
	void setViewport(int width, int height);

	void lookCamera(const glm::vec3 &pos, const glm::vec3 &Ref, bool RotateAroundReference);
	void draw();
	// interface functions
	void setMouseLeft(bool flag) { modelStates.mouseLeftDown = flag; };
	void setMouseRight(bool flag) { modelStates.mouseRightDown = flag; };
	void setMousePosition(int x, int y) { mouseX = GLfloat(x); mouseY = GLfloat(y); };
	void changeLight() { modelStates.lighting ^= 1; };
	void changeShowNormals() { modelStates.showNormals ^= 1; };
	void rotateCamera(int x, int y);
	void zoomCamera(int dist);

	
	glModel(void);
	~glModel(void);
	// set functions
	void setWinHandle(HWND _handle) { 
		handle = _handle; 
		MainShader.setWinHandle(&_handle);	
		MainWithLightShader.setWinHandle(&_handle);
		NormalVecShader.setWinHandle(&_handle);
	};
	void setDevContext(HDC _newDevContext) { hDevC = _newDevContext; };
};
#endif
