#include "WinCtrl.h"

inline int WinCtrl::close() { ::DestroyWindow(handle); return 0; }
inline int WinCtrl::command(int id, int cmd, LPARAM msg) { return 0; }
inline int WinCtrl::contextMenu(HWND handle, int x, int y) { return 0; }
inline int WinCtrl::create() { return 0; }
inline int WinCtrl::destroy() { return 0; }
inline int WinCtrl::enable(bool flag) { return 0; }
inline int WinCtrl::eraseBkgnd(HDC hdc) { return 0; }
inline int WinCtrl::hScroll(WPARAM wParam, LPARAM lParam) { return 0; }
inline int WinCtrl::keyDown(int key, LPARAM lParam) { return 0; }
inline int WinCtrl::keyUp(int key, LPARAM lParam) { return 0; }
inline int WinCtrl::lButtonDown(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::lButtonUp(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::mButtonDown(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::mButtonUp(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::mouseMove(WPARAM keyState, int x, int y) { return 0; }
inline int WinCtrl::mouseWheel(int state, int delta, int x, int y) { return 0; }
inline int WinCtrl::notify(int id, LPARAM lParam) { return 0; }
inline int WinCtrl::paint() { return 0; }
inline int WinCtrl::rButtonDown(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::rButtonUp(WPARAM wParam, int x, int y) { return 0; }
inline int WinCtrl::size(int w, int h, WPARAM type) { return 0; }
inline int WinCtrl::timer(WPARAM id, LPARAM lParam) { return 0; }
inline int WinCtrl::vScroll(WPARAM wParam, LPARAM lParam) { return 0; }

WinCtrl::WinCtrl() : handle(0)
{
}

WinCtrl::~WinCtrl()
{
    DestroyWindow(handle);
}

