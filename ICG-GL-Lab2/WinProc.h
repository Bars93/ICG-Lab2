#ifndef WINPROC_H
#define WINPROC_H
#include <Windows.h>
#include "WinApp.h"
extern WinApp *AppPtr;
LRESULT CALLBACK WndProcEXT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
