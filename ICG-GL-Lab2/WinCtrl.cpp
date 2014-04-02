#include "WinCtrl.h"




WinCtrl::WinCtrl() : handle(0)
{
}

WinCtrl::~WinCtrl()
{
    DestroyWindow(handle);
}

