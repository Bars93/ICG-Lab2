#ifndef GLMODEL_H
#define GLMODEL_H
#include "glHeaders.h"
#include "glShader.h"
#include <vector>
#include <FreeImage.h>
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
	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
} lightSourceP;
typedef struct materialSourceParams {
	glm::vec3 refAmbient;
	glm::vec3 refDiffuse;
	glm::vec3 refSpecular;
	glm::vec3 emission;
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
	USHORT useShadersflg : 1;
	USHORT lighting: 1; // lighting on
	USHORT showNormals : 1; // draw normals
	USHORT userPathMode : 1;
	USHORT brokenPathMode : 1;
	USHORT reCalcNormals : 1; // recalculate normals
	USHORT loadPath : 1; // load user path
	USHORT loadSlice : 1; // load user slice
	USHORT bufferBind : 1; // buffers are bind
	USHORT readyToDraw : 1; // frame ready to draw
	USHORT loadBreaks : 1; // new path breaks
	USHORT wireframe : 1; // wireframe mode
	USHORT texMode : 1; // *
	USHORT smoothNormals : 1;
	USHORT smoothEdges : 1;
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
	const enum vaoIDs { mainVA, brokenDrawVA, VAOcount };
	// VAOs array
	GLuint VAOs[VAOcount];	

	// buffers IDs enumeration
	const enum bufferIDs {pathDrawID, texBufID, brokenPathDrawID, matrMapBufID, bufCount };
	// vertex attributes enumeration
	const enum vertAttribs { vertPosition, normalVec, attribCount };
	const enum NDvertAttribs { NDvertPosition, NDnormalVec, NDattribCount };
	// lights enumeration
	const enum lightsVecs { mLight0, lightsCount };

	lightSourceP lightsP[lightsCount];
	materialSourceP materialP;
	// light positions array

	GLfloat totalPathLength, totalBrokenPathLength;
	// buffers IDs array
	GLuint buffers[bufCount];

	GLint NDshaderAttribs[NDattribCount];
	//vertex attrib IDs array
	GLint shaderAttribLocs[attribCount];
	GLuint vertexCount, BPvertexCount; // count of vertexes

	// buffer storages;
	vector<glm::vec3> normalBuf; // normal buffer
	//vector<glm::vec3> axisBuf; // axis vectors (depr)
	vector<glm::vec3> userPath, calcPath; // user define path (original)
	vector<glm::vec3> sidesVerteces;
	vector<glm::vec3> sidesNormals;
	vector<GLfloat> pathBreaks; // path breaks (in percents)
	vector<GLfloat> rotAngles;
	vector<GLfloat> pathLengthes;
	vector<glm::vec3> slicePath, drawPath,  drawSlicePath; // calculated path
	vector<glm::vec3> BPnormalBuf;
	vector<glm::vec2> slice; // slice
	vector<GLuint> breaksMap;
	vector<glm::vec2> texCoords, BPtexCoords;

	const enum texIDs { cobTex, texCount };
	GLuint modelTextures[texCount];
	GLuint sliceVertCount, // vertices count of slice
		pathSteps, brokenPathSteps, // path steps
		normVecCount, BPnormVecCount; // normal vectors count
	sTransform matrices;
	bitStates modelStates; // current model states
	// swapping buffer (DOUBLEBUFFER)
	void swapDevBuffers();
	// load textures
	bool LoadTexture(LPCWSTR filename,FIBITMAP **dib, int &tWidth, int &tHeight, int &tBPP); 
	// binding textures
	bool bindTexture(); 
	// initialization of textures
	bool initTextures(const GLboolean bGenMipMaps, GLint curAF); 
	// rebind shaders attributes
	void RebindShaderAttributes(GLuint &currentVAO_ID, GLuint &currentBufID, GLuint vertCount, GLuint sideVertCount); 
	// rebind normals draw shader attributes
	void RebindNormalShaderAttributes(GLuint &currentVAO_ID, GLuint &currentBufID, GLuint vertCount, GLuint sideVertCount);

	void unbindShaderAttributes();
	
	void unbindNormalShaderAttributes();
	// open file (load) dialog
	bool loadDialog(LPCWSTR dialogTitle, LPWSTR filename, LPCWSTR filter); 
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
	void calculateSurfsWithBreaks();
	void calculatingNormals(vector<glm::vec3> &vertecies, vector<glm::vec3> &normals); // *
	void smoothingNormals(vector<glm::vec3> &nrmbuf);
	void TexturePath();
	void TextureBPPath();
	void getTexCoords(const vector<glm::vec3> &vertecies, vector<glm::vec2> &texCoords, GLfloat pathLen);
	void getSideTexCoords(const vector<glm::vec3> &vertecies, vector<glm::vec2> &texCoords, vector<glm::vec3> &path);
public:
	bool initBuffers();
	bool initBuffersBP();
	bool init();    // initialize OpenGL states
	void checkReadyToDraw();
	void loadFileData();
	void loadSliceData();
	void loadPositionsData();
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
	void changeSmoothMode(USHORT state) { modelStates.smoothNormals = state ? 1 : 0; };
	void changeRecalcNormalsFlg() { modelStates.reCalcNormals = 1; };
	void changeTextureMode(USHORT state) { modelStates.texMode = state ? 1 : 0; };

	bool getShowNormals() const { return modelStates.showNormals; };
	bool getWireframe() const { return modelStates.wireframe; };
	bool getLighting() const { return modelStates.lighting; };
	bool getSmoothMode() const { return modelStates.smoothNormals; };
	bool getUserPathMode() const { return modelStates.userPathMode; };
	bool getBrokenPathMode() const { return modelStates.brokenPathMode; };
	bool getSliceLoaded() const { return modelStates.loadSlice; };
	bool getPathLoaded() const { return modelStates.loadPath; };
	bool getBreaksLoaded() const { return modelStates.loadBreaks; };
	bool getTextureMode() const { return modelStates.texMode; };

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
