#ifndef GLCTRL_H
#define GLCTRL_H
#include "WinCtrl.h"
#include "glView.h"
#include "glModel.h"
class glCtrl : public WinCtrl
{
	//static void threadFunction(void* arg);      // static thread function, it will route to member function, runThread()
	//void runThread();                           // thread for OpenGL rendering

	glModel* modelGL;                           
	glView* viewGL;                             
	HANDLE threadHandle;
	unsigned int threadId;
//	volatile bool loopFlag;                     // rendering loop flag
	//bool resizeFlag;
	int clientWidth;                            // width of client area
	int clientHeight;                           // height of client area
public:
	glCtrl(glView *view, glModel *model);
	~glCtrl(void);

	int close();                                // close the RC and destroy OpenGL window
	int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
	int create();                               // create RC for OpenGL window and start new thread for rendering
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
};
#endif

