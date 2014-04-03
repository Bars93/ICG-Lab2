#pragma once
#ifdef _WIN32 // Windows only
#ifndef WINCTRL_H
#define WINCTRL_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
class WinCtrl
{
protected:
	HWND handle; 
private:
	ULONG32 MOUSE_X, MOUSE_Y;
public:
	virtual int close();                                    // for WM_CLOSE
	virtual int command(int id, int cmd, LPARAM msg);       // for WM_COMMAND: id, cmd, msg
	virtual int contextMenu(HWND handle, int x, int y);     // for WM_CONTEXTMENU: handle, x, y
	virtual int create();                                   // for WM_CREATE
	virtual int destroy();                                  // for WM_DESTROY
	virtual int enable(bool flag);                          // for WM_ENABLE: flag(true/false)
	virtual int eraseBkgnd(HDC hdc);                        // for WM_ERASEBKGND: HDC
	virtual int hScroll(WPARAM wParam, LPARAM lParam);      // for WM_HSCROLL
	virtual int keyDown(int key, LPARAM lParam);            // for WM_KEYDOWN: keyCode, detailInfo
	virtual int keyUp(int key, LPARAM lParam);              // for WM_KEYUP: keyCode, detailInfo
	virtual int lButtonDown(WPARAM state, int x, int y);    // for WM_LBUTTONDOWN: state, x, y
	virtual int lButtonUp(WPARAM state, int x, int y);      // for WM_LBUTTONUP: state, x, y
	virtual int mButtonDown(WPARAM state, int x, int y);    // for WM_MBUTTONDOWN: state, x, y
	virtual int mButtonUp(WPARAM state, int x, int y);      // for WM_MBUTTONUP: state, x, y
	virtual int mouseMove(WPARAM state, int x, int y);      // for WM_MOUSEMOVE: state, x, y
	virtual int mouseWheel(int state, int d, int x, int y); // for WM_MOUSEWHEEL: state, delta, x, y
	virtual int notify(int id, LPARAM lParam);              // for WM_NOTIFY: WinCtrlID, NMHDR
	virtual int paint();                                    // for WM_PAINT
	virtual int rButtonDown(WPARAM wParam, int x, int y);   // for WM_RBUTTONDOWN: state, x, y
	virtual int rButtonUp(WPARAM wParam, int x, int y);     // for WM_RBUTTONUP: state, x, y
	virtual int size(int w, int h, WPARAM wParam);          // for WM_SIZE: width, height, type(SIZE_MAXIMIZED...)
	virtual int timer(WPARAM id, LPARAM lParam);            // for WM_TIMER: ID, ptr to callback ftn
	virtual int vScroll(WPARAM wParam, LPARAM lParam);      // for WM_VSCROLL
	WinCtrl(void);
	~WinCtrl(void);
	/*Set functions*/
	void setHandle(HWND handle);                            // set window handle
	/*get functions*/
};
inline void WinCtrl::setHandle(HWND hwnd) { handle = hwnd; }
#endif
#endif
