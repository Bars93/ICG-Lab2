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
using glm::mat4;
typedef struct lightSourceParams {
	glm::vec4 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
} lightSourceP;
typedef struct materialSourceParams {
	glm::vec3 refAmbient;
	glm::vec3 refDiffuse;
	glm::vec3 refSpecular;
	float Shininess;
} materialSourceP;
typedef struct tagTransform {
	mat4 Model;
	mat4 View;
	mat4 Projection;
	mat4 ModelView;
	mat4 ModelViewProjection;
	mat4 Normal;
} sTransform;
using std::vector;
struct bitStates {
	USHORT mouseLeftDown : 1; // left mouse button pressed
	USHORT mouseRightDown : 1; // right mouse button pressed
	USHORT useShadersflg : 1; // deprecated in next release
	USHORT shaderInUse : 1; // shaders active
	USHORT lighting: 1; // lighting on
	USHORT showNormals : 1; // draw normals
	USHORT remake : 1; // recalculate 
	USHORT loadPath : 1; // load user path
	USHORT loadSlice : 1; // load user slice
	USHORT bufferBind : 1; // buffers are bind
	USHORT readyToDraw : 1; // frame ready to draw
	USHORT loadBreaks : 1; // new path breaks
	USHORT wireframe : 1; // wireframe mode
	USHORT tesselation : 1; // tesselation mode
	USHORT material : 1; // materials on
};
class glModel
{
private:
	WCHAR errorString[1024]; // errorString output string of error
	GLfloat width, height; // width and height of client window space 
	HWND handle; // window handle
	HDC hDevC; // validated device context
	GLfloat normalDirect; // normals direction
	// mouse state
	GLfloat mouseX, mouseY; // mouse positions
	// camera properties
	glm::vec3 X, // Camera's X axis 
		Y, // Camera's Y axis
		Z,  // Camera's Z axis
		Position, // Camera's positions
		Reference; // Camera's reference of view
	// shader's variables
	glShader MainShader, // main draw shaders
		NormalVecShader; // normals draw shaders
	GLuint NShaderProg;
	GLuint ProgLight,  // Main program ID
		ProgNormals; // normals draw program ID
	// VAOs IDs enumeration
	const enum vaoIDs { mainVA, VAOcount };
	// VAOs array
	GLuint VAOs[VAOcount];	

	// buffers IDs enumeration
	const enum bufferIDs {pathDrawID, matrMapBufID, bufCount };
	// vertex attributes enumeration
	const enum vertAttribs { vertPosition, normalVec, attribCount };
	const enum NDvertAttribs { NDvertPosition, NDnormalVec, NDattribCount };
	// shaders uniforms enumeration
	const enum uniformVars { 
		CameraPos,
		vertColor3f,
		light0position,
		light0ambient,
		light0diffuse,
		light0specular,
		matAmbient,
		matDiffuse,
		matSpecular,
		matShininess,
		turnLight,
		uniformCount 
	};
	// lights enumeration
	const enum lightsVecs { mLight0, lightsCount };

	lightSourceP lightsP[lightsCount];
	materialSourceP materialP;
	// light positions array

	float totalPathLength;
	// buffers IDs array
	GLuint buffers[bufCount];

	//shader uniform IDs array
	GLint shaderUniformLocs[uniformCount];
	GLint NDshaderAttribs[NDattribCount];
	//vertex attrib IDs array
	GLint shaderAttribLocs[attribCount];
	GLuint vertexCount; // count of vertexes

	// buffer storages;
	vector<glm::vec3> normalBuf; // normal buffer
	vector<GLuint> indices; // indices buffer
	//vector<glm::vec3> axisBuf; // axis vectors (depr)
	vector<glm::vec3> userPath; // user define path (original)
	vector<glm::vec3> sidesVerteces;
	vector<glm::vec3> sidesNormals;
	vector<GLfloat> pathBreaks; // path breaks (in percents)
	vector<glm::vec3> slicePath, drawPath; // calculated path
	vector<glm::vec3> normVBuf;
	vector<glm::vec2> slice; // slice
	GLuint sliceVertCount, // vertices count of slice
		sliceSize, // slice size
		pathSteps, // path steps
		normVecCount; // normal vectors count
	sTransform matrices;
	bitStates modelStates; // current model states
	// swapping buffer (DOUBLEBUFFER)
	void swapDevBuffers();
	// load textures
	bool LoadTexture(LPCWSTR filename); 
	// binding textures
	bool bindTexture(); 
	// initialization of textures
	bool initTextures(); 
	// rebind shaders attributes
	void RebindShaderAttributes(); 
	// rebind normals draw shader attributes
	void RebindNormalShaderAttributes();

	void unbindShaderAttributes();
	
	void unbindNormalShaderAttributes();
	// open file (load) dialog
	bool loadDialog(LPCWSTR dialogTitle, LPWSTR filename); 
	// calculate normal of surface
	glm::vec3 calculateNormal(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c); 
	// calculate specific projection of point
	glm::vec3 projectPoint(const glm::vec3 &point,glm::mat3 &rotMatr,glm::vec3 &translateVector);
	// calculate view matrix
	void calculateViewMatrix(); 

	void initLightingMode();
	void bindLightShaderVariables();
	void unbindAllShaders();
	void calculateSurfsWithoutBreaks();
public:
	void calculateSurfaces();
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
	void changeLight(USHORT state) { modelStates.lighting = state ? 1 : 0; };
	void changeShowNormals(USHORT state) { modelStates.showNormals = state ? 1 : 0; };
	void changeShowMode(USHORT state) { modelStates.wireframe = state ? 1 : 0; };

	bool getShowNormals() const { return modelStates.showNormals; };
	bool getWireframe() const { return modelStates.wireframe; };
	bool getLighting() const { return modelStates.lighting; };
	void rotateCamera(int x, int y);
	void zoomCamera(int dist);

	
	glModel(void);
	~glModel(void);
	// set functions
	void setWinHandle(HWND _handle) { 
		handle = _handle; 
		MainShader.setWinHandle(&_handle);	
		NormalVecShader.setWinHandle(&_handle);
	};
	void setDevContext(HDC _newDevContext) { hDevC = _newDevContext; };
};
#endif
