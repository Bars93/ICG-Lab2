#ifndef GLCTRL_H
#define GLCTRL_H
#include "WinCtrl.h"
#include "glView.h"
#include "glModel.h"
class glCtrl : public WinCtrl
{
	glModel modelGL;                           
	glView viewGL;    
	int clientWidth;                            // width of client area
	int clientHeight;                           // height of client area
	bool GLinit;
	hostInfo *curHostInfo;
	long int lastSecond;
	int iCurrentFPS;
	float iFPScount;
public:
	glCtrl();
	glCtrl(glView *view, glModel *model);
	~glCtrl(void);
	int close();                                // close the RC and destroy OpenGL window
	int command(HWND hwnd,int id, int cmd, LPARAM msg);   // for WM_COMMAND
	int create();                               // create RC for OpenGL window and start new thread for rendering
	int initOpenGL(HDC *hdc,HWND *handle, HINSTANCE *hInst);
	void updateControls();
	int destroy();
	int paint();
	int lButtonDown(WPARAM state, int x, int y);
	int lButtonUp(WPARAM state, int x, int y);
	int rButtonDown(WPARAM state, int x, int y);
	int rButtonUp(WPARAM state, int x, int y);
	int mouseMove(WPARAM state, int x, int y);
	int mouseWheel(int state, int d, int x, int y);
	int keyDown(int key, LPARAM lParam);
	int size(int width, int height, WPARAM type);
	int wActivate(int aCmd,int minimized, HWND hWnd);
	float getFPS() const { return iFPScount; };
};
#endif

