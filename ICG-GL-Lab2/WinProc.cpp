#include "WinProc.h"
#include "WinCtrl.h"
LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT returnValue = 0;        // return value

	// find controller associated with window handle
	static WinCtrl *ctrl;
	ctrl = (WinCtrl*) GetWindowLongPtr(hwnd, GWL_USERDATA);

	if(msg == WM_NCCREATE)  // Non-Client Create
	{
		// WM_NCCREATE message is called before non-client parts(border,
		// titlebar, menu,etc) are created. This message comes with a pointer
		// to CREATESTRUCT in lParam. The lpCreateParams member of CREATESTRUCT
		// actually contains the value of lpPraram of CreateWindowEX().
		// First, retrieve the pointrer to the controller specified when
		// WinApp is setup.
		ctrl = (WinCtrl*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		ctrl->setHandle(hwnd);

		// Second, store the pointer to the Controller into GWL_USERDATA,
		// so, other messege can be routed to the associated Controller.
		::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)ctrl);

		return DefWindowProc(hwnd, msg, wParam, lParam);

	}
	// check NULL pointer, because GWL_USERDATA is initially 0, and
	// we store a valid pointer value when WM_NCCREATE is called.
	if(!ctrl)
		return DefWindowProc(hwnd, msg, wParam, lParam);

	// route messages to the associated controller
	switch(msg)
	{
	case WM_CREATE:
		returnValue = ctrl->create();
		break;

	case WM_SIZE:
		returnValue = ctrl->size(LOWORD(lParam), HIWORD(lParam), (int)wParam);    // width, height, type
		break;

		//case WM_ENABLE:
		//    {
		//    bool flag;
		//    if(wParam) flag = true;
		//    else flag = false;
		//    returnValue = ctrl->enable(flag);  // TRUE or FALSE
		//    break;
		//    }
	case WM_MOUSEWHEEL: 
		{
			returnValue = ctrl->mouseWheel(GET_KEYSTATE_WPARAM(wParam), // Keystate
				GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, // delta of moving
				GET_X_LPARAM(lParam), // x coordinate
				GET_Y_LPARAM(lParam)); // y coordinate
			SendMessage(hwnd,WM_PAINT,NULL,NULL);
			break;
		}
	case WM_PAINT:
		InvalidateRect( hwnd, NULL, FALSE ); 
		ctrl->paint();
		returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	case WM_NCPAINT:  /* 0x0085 */
		/* Need to update the border of this window */
		returnValue = DefWindowProc( hwnd, msg, wParam, lParam );
		/* Pass it on to "DefWindowProc" to repaint a standard border */
		break;
	case WM_COMMAND:
		returnValue = ctrl->command(LOWORD(wParam), HIWORD(wParam), lParam);   // id, code, msg
		break;

	case WM_MOUSEMOVE:
		returnValue = ctrl->mouseMove(wParam, LOWORD(lParam), HIWORD(lParam));  // state, x, y
		SendMessage(hwnd,WM_PAINT,NULL,NULL);
		break;

	case WM_CLOSE:
		returnValue = ctrl->close();
		break;

	case WM_DESTROY:
		returnValue = ctrl->destroy();
		break;

		//case WM_SYSCOMMAND:
		//    //returnValue = ctrl->sysCommand(wParam, lParam);
		//    returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
		//    break;

		//    // Disable Alt-F4 and screensavers
		//    switch (wParam & 0xfff0)
		//    {
		//    case SC_CLOSE:
		//    case SC_SCREENSAVE:
		//       returnValue = 0;
		//        break;

		//    default:
		//        returnValue = DefWindowProc(hwnd, msg, wParam, lParam);
		//        break;
		//    }
		//    break;


		//case WM_CHAR:
		//    returnValue = ctrl->char(wParam, lParam);   // route keycode
		//    break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		returnValue = ctrl->keyDown((int)wParam, lParam);                       // keyCode, keyDetail
		SendMessage(hwnd,WM_PAINT,NULL,NULL);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		returnValue = ctrl->keyUp((int)wParam, lParam);                         // keyCode, keyDetail
		SendMessage(hwnd,WM_PAINT,NULL,NULL);
		break;

	case WM_LBUTTONDOWN:
		returnValue = ctrl->lButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_LBUTTONUP:
		returnValue = ctrl->lButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_RBUTTONDOWN:
		returnValue = ctrl->rButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_RBUTTONUP:
		returnValue = ctrl->rButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_MBUTTONDOWN:
		returnValue = ctrl->mButtonDown(wParam, LOWORD(lParam), HIWORD(lParam)); // state, x, y
		break;

	case WM_MBUTTONUP:
		returnValue = ctrl->mButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));   // state, x, y
		break;

	case WM_HSCROLL:
		returnValue = ctrl->hScroll(wParam, lParam);
		break;

	case WM_VSCROLL:
		returnValue = ctrl->vScroll(wParam, lParam);
		break;

	case WM_TIMER:
		returnValue = ctrl->timer(LOWORD(wParam), HIWORD(wParam));
		break;

	case WM_NOTIFY:
		returnValue = ctrl->notify((int)wParam, lParam);                        // controllerID, lParam
		break;

	case WM_CONTEXTMENU:
		returnValue = ctrl->contextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));    // handle, x, y (from screen coords)
		break;

		//case WM_ERASEBKGND:
		//	returnValue = ctrl->eraseBkgnd((HDC)wParam);                            // handle to device context
		//	break;

	default:
		returnValue = ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return returnValue;
}