#ifndef GLMODEL_H
#define GLMODEL_H
#include <GL/glew.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#ifdef _WIN32
#include <Windows.h>
#include <GL/wglew.h>
#include <GL/wglext.h>
#else
#include <GL/glxew.h>
#endif
#include <vector>

class glModel
{
	std::vector<POINT> figure;
	std::vector<std::vector<POINT>> figures;
	std::vector<COLORREF> figColor;
	std::vector<std::vector<COLORREF>> figsColor;
	bool mouseLeftDown;
	bool mouseRightDown;
	int mouseX;
	int mouseY;
	float cameraAngleX;
	float cameraAngleY;
	float cameraDistance;
	GLuint vaID, voID;
public:
	void init();                                    // initialize OpenGL states
	//void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
	void setViewport(int width, int height);
	void draw();

	void setMouseLeft(bool flag) { mouseLeftDown = flag; };
	void setMouseRight(bool flag) { mouseRightDown = flag; };
	void setMousePosition(int x, int y) { mouseX = x; mouseY = y; };

	//void rotateCamera(int x, int y);
	//void zoomCamera(int dist);
	glModel(void);
	~glModel(void);
};
#endif
