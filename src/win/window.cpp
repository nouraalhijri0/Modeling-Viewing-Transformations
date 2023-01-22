#include "window.h"

#include "glue.h"

#include <mmsystem.h>
#include <shellapi.h>
#if _WIN32_WINNT >= 0x0605
#include <ShellScalingApi.h>
#endif

#pragma comment(lib, "winmm.lib")
#if _WIN32_WINNT >= 0x0605
#pragma comment(lib, "shcore.lib")
#endif

// ngan.nguyen: add GLFW
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <stdio.h>
GLFWwindow* _NULLABLE _window;
// ngan.nguyen: end
//****************************************************************************/

/*
 * Default window width (in pixels at 96 DPI; scaled for other sizes).
 */
#ifndef WINDOW_WIN_W
#define WINDOW_WIN_W 800
#endif

/*
 * Default window height (in pixels at 96 DPI; scaled for other sizes).
 */
#ifndef WINDOW_WIN_H
#define WINDOW_WIN_H 600
#endif

/*
 * Default window title.
 */
#ifndef WINDOW_WIN_TITLE
#define WINDOW_WIN_TITLE "CS248"
#endif

/*
 * Set if the window should be resizable (otherwise it is fixed to the width
 * and height defined above).
 * 
 * TODO: disable arbitrary resize and support fixed options?
 */
#ifndef WINDOW_WIN_RESIZE
#define WINDOW_WIN_RESIZE 0
#endif

/*
 * Tunes the Windows timer and sleep period. Sets both the minimum timer
 * resolution and the sleep time between frame updates. A value of zero will
 * give a glitch-free playback at the expense of CPU (and battery). 10 will
 * drop CPU usage to zero but struggle to maintain framerate (but an even
 * paced struggle). 1-5 is a good choice (2 being a good compromise).
 */
#ifndef WINDOW_SLEEP_PERIOD
#define WINDOW_SLEEP_PERIOD 2
#endif

/**
 * Mask for the \c lParam in Windows key events for the previous state (zero
 * if it was up previously, otherwise it was down). Used to filter repeats.
 */
#ifndef WM_KEY_WAS_DOWN
#define WM_KEY_WAS_DOWN (1 << 30)
#endif

//************************ Windows Hi-DPI Workarounds ************************/

/**
 * Helper to retrieve a function pointer given its name and library, e.g.:
 * \code
 *	typedef void (*myFunc_t)();
 *	myFunc_t myFunc = findFunction<myFunc_t>("myLib.dll", "myFunc");
 *	if (myFunc != NULL) {
 *		myFunc();
 *	}
 * \endcode
 * 
 * \param[in] lib DLL/library name
 * \param[in] name function name
 * \return function pointer (or \c null if the lookup fails)
 * \tparam T function prototype type
 */
template<typename T>
static T findFunction(LPCTSTR const lib, LPCSTR const name) {
	if (lib && name) {
		HMODULE dll = GetModuleHandle(lib);
		if (dll) {
			return reinterpret_cast<T>(GetProcAddress(dll, name));
		}
	}
	return NULL;
}

/**
 * \def MAKE_DYNAMIC_FUNC
 * Helper to define and create a dynamic function, e.g.:
 * \code
 *	MAKE_DYNAMIC_FUNC("myLib.dll", myFunc);
 * \endcode
 * The above is a shortcut for:
 * \code
 *	myFunc_t myFunc = findFunction<myFunc_t>("myLib.dll", "myFunc");
 * \endcode
 * 
 * \param lib library name
 * \param name function name
 */
#ifndef MAKE_DYNAMIC_FUNC
#define MAKE_DYNAMIC_FUNC(lib, name) static name##_t name = findFunction<name##_t>(TEXT(lib), #name)
#endif

#if _WIN32_WINNT < 0x0605
/**
 * \def WM_DPICHANGED
 * Windows message when the application window's DPI changes. Supported in
 * Windows 8.1 onwards.
 */
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

 /**
  * \def WM_GETDPISCALEDSIZE
  * Windows message when to \e calculate the change for \c #WM_DPICHANGED.
  * Supported in Windows 10 Creators Update (1703) onwards.
  */
#ifndef WM_GETDPISCALEDSIZE
#define WM_GETDPISCALEDSIZE 0x02E4
#endif

/**
 * \def DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
 * Per Monitor v2, the only setting of interest for DPI aware applications
 * that use other Windows controls (including the title bar). Only supported
 * in Windows 10 Creators Update (1703) onwards (though not programatically).
 * 
 * \sa https://msdn.microsoft.com/library/windows/desktop/mt791579(v=vs.85).aspx
 */
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 reinterpret_cast<HANDLE>(-4)
#endif

/**
 * Prototype for \c SetProcessDpiAwarenessContext, allowing DPI awareness to
 * be set programatically. Only supported in Windows 10 Creators Update (1703)
 * onwards (and with matching SDK).
 * 
 * \note The \e correct way of setting this is in the manifest, complicated by
 * the requirement to call \c EnableNonClientDpiScaling in earlier Win10
 * versions. We assume hi-DPI displays will be used on up-to-date installs.
 *
 * \sa https://msdn.microsoft.com/en-us/library/windows/desktop/dn302122(v=vs.85).aspx
 */
typedef BOOL (WINAPI *SetProcessDpiAwarenessContext_t)(HANDLE);

/**
 * Prototype for \c GetDpiForSystem, returning the default system DPI. Only supported
 * in Windows 10 Anniversary Update (1607) onwards.
 *
 * \sa https://msdn.microsoft.com/en-us/library/windows/desktop/mt748623(v=vs.85).aspx
 */
typedef UINT (WINAPI *GetDpiForSystem_t)();

/**
 * Prototype for \c GetDpiForWindow, returning the passed window handle's DPI.
 * Only supported in Windows 10 Anniversary Update (1607) onwards.
 *
 * \sa https://msdn.microsoft.com/en-us/library/windows/desktop/mt748624(v=vs.85).aspx
 */
typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);

/**
 * Prototype for \c AdjustWindowRectExForDpi, calculating the window bounds
 * for a given style. Only supported in Windows 10 Anniversary Update (1607)
 * onwards.
 * 
 * \sa https://msdn.microsoft.com/en-us/library/windows/desktop/mt748618(v=vs.85).aspx
 */
typedef BOOL (WINAPI *AdjustWindowRectExForDpi_t)(LPRECT, DWORD, BOOL, DWORD, UINT);

/*
 * Adds the polyfills.
 */
MAKE_DYNAMIC_FUNC("User32.dll", SetProcessDpiAwarenessContext);
MAKE_DYNAMIC_FUNC("User32.dll", GetDpiForSystem);
MAKE_DYNAMIC_FUNC("User32.dll", GetDpiForWindow);
MAKE_DYNAMIC_FUNC("User32.dll", AdjustWindowRectExForDpi);
#endif

//****************************************************************************/

/**
 * Helper to convert a \c Handle to a window.
 */
#define TO_HND(win) reinterpret_cast<window::Handle>(win)
 /**
  * Helper to convert a window to a \c Handle.
  */
#define TO_WIN(hnd) reinterpret_cast<HWND>(hnd)

namespace impl {
//**************************** Windows Event Loop ****************************/

/**
 * Helper to toggle between windowed mode and fullscreen.
 * 
 * \todo tested only on Windows 10 so far
 * 
 * \param[in] hWnd main application window handle
 * \param[in] exit \c true if the toggle can only \e exit fullscreen
 */
static void toggleFullscreen(HWND const hWnd, bool const exit = false) {
	/*
	 * Note: stores the *single* window state of the main application (and
	 * doesn't track that the window handle is the same one).
	 */
	static struct FSState {
		FSState() : fullscrn(false) {}
		bool fullscrn; // whether the last call set fullscreen
		LONG wndStyle; // the windowed mode style (to restore)
		LONG extStyle; // the windowed mode extended style (to restore)
		RECT origRect; // the windowed mode original size and position
	} state;
	if (state.fullscrn == false) {
		if (!exit) {
			/*
			 * Stores the current window styles, unsets the menu bar and any
			 * frame features, then fits the undecorated content to the
			 * nearest monitor bounds.
			 */
			state.wndStyle = GetWindowLong(hWnd, GWL_STYLE);
			state.extStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
			GetWindowRect(hWnd, &state.origRect);
			MONITORINFO info; info.cbSize = sizeof(info);
			if (GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &info)) {
				SetWindowLong(hWnd, GWL_STYLE,   state.wndStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowLong(hWnd, GWL_EXSTYLE, state.extStyle & ~WS_EX_OVERLAPPEDWINDOW);
				SetWindowPos(hWnd, HWND_TOP, info.rcMonitor.left, info.rcMonitor.top,
					info.rcMonitor.right - info.rcMonitor.left,
						info.rcMonitor.bottom - info.rcMonitor.top, SWP_FRAMECHANGED);
				state.fullscrn = !state.fullscrn;
			}
		}
	} else {
		/*
		 * Restores styles and size.
		 */
		SetWindowLong(hWnd, GWL_STYLE,   state.wndStyle);
		SetWindowLong(hWnd, GWL_EXSTYLE, state.extStyle);
		SetWindowPos (hWnd, NULL, state.origRect.left, state.origRect.top,
			state.origRect.right - state.origRect.left,
				state.origRect.bottom - state.origRect.top, SWP_FRAMECHANGED);
		state.fullscrn = !state.fullscrn;
	}
}

/**
 * Queries a window's DPI.
 * 
 * \param[in] hWnd window handle (or \c null for the default window)
 * \return best guess at the DPI
 */
static UINT getWindowDpi(HWND const hWnd = NULL) {
	/*
	 * GetDpiForMonitor and associated calls are removed from modern SDKs, and
	 * newer API calls aren't available on older Windows. This works. Sort of.
	 * The fallback (seemingly) grabs the default monitor and ignores any
	 * updated scale preferences, but is better than creating a tiny window on
	 * hi-DPI systems.
	 * 
	 * TODO: add GetDpiForMonitor for older Windows anyway
	 */
	UINT dpi = 0;
	if (hWnd && GetDpiForWindow != NULL) {
		dpi = GetDpiForWindow(hWnd);
	} else {
		if (GetDpiForSystem != NULL) {
			dpi = GetDpiForSystem();
		} else {
			HDC hdc = GetDC(hWnd);
			if (hdc) {
				dpi = max(GetDeviceCaps(hdc, LOGPIXELSX),
						  GetDeviceCaps(hdc, LOGPIXELSY));
				ReleaseDC(hWnd, hdc);
			}
		}
	}
	if (dpi == 0) {
		dpi = USER_DEFAULT_SCREEN_DPI;
	}
	return dpi;
}

/**
 * Standard \c WndProc function for Windows events, passing key presses, mouse
 * events, etc., to the different \c impl internal functions.
 * 
 * \param[in] hWnd window handle
 * \param[in[ uMsg message type
 */
LRESULT CALLBACK windowEvents(HWND const hWnd, UINT const uMsg, WPARAM const wParam, LPARAM const lParam) {
	switch (uMsg) {
	case WM_SIZE:
		/*
		 * See note about the swap in surfaceResize and WM_WINDOWPOSCHANGED,
		 * so look into a better way of doing this.
		 */
		if (wParam != SIZE_MINIMIZED) {
			//impl::surfaceResize(LOWORD(lParam), HIWORD(lParam), dpiScale);
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND:
		/*
		 * Return non-zero to tell Windows we handled the erase (a no-op).
		 */
		return 1;
	case WM_WINDOWPOSCHANGED:
		/*
		 * Early on, when first experimenting with ANGLE, moving the window
		 * was causing smearing artifacts. The workaround was to recopy the
		 * colour buffer for every message. This no longer appears necessary
		 * for moving (at a guess, the DX11 backend changed) but resizing and
		 * DPI changes look better. The (SWP_NOSIZE | SWP_NOMOVE) corresponds
		 * to window activation and DPI/monitor changes.
		 * 
		 * A true fix would be to redraw, not just swap buffers, allowing for
		 * live resizing. This is a lightweight compromise.
		 */
		if ((reinterpret_cast<WINDOWPOS*>(lParam)->flags &  SWP_NOSIZE) == 0 ||
			(reinterpret_cast<WINDOWPOS*>(lParam)->flags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE))
		{
			/*
			 * Hmm, I'm not happy still with this. It looks to cause massive
			 * flickering when going from different DPI monitors in some
			 * configs. The swap is commented out for now.
			 * 
			 * if (eglDisplay != EGL_NO_DISPLAY && eglSurface != EGL_NO_SURFACE) {
			 *     eglSwapBuffers(eglDisplay, eglSurface);
			 * }
			 */
		}
		break;
	case WM_KEYUP:
		/*
		 * F11 toggles between fullscreen and windowed mode; ESC exits
		 * fullscreen (but does nothing if windowed).
		 * 
		 * TODO: other function keys for 4:3, 16:9, etc.?
		 */
		if (wParam == VK_F11) {
			toggleFullscreen(hWnd);
		} else {
			if (wParam == VK_ESCAPE) {
				toggleFullscreen(hWnd, true);
			}
		}
		break;
	#ifdef WM_DPICHANGED
		/*
		 * Handles the DPI change when moving between monitors (Win8.1+). The
		 * incoming RECT contains the suggested size, which works better than
		 * taking the DPI and calculating (given, when switching between
		 * normal to high to normal, increasingly shrinks the window). It's
		 * still not 100%, but is at least consistent when moving between
		 * monitors.
		 * 
		 * TODO: older Windows (pre-Win10 1703) appear to get the size calculation wrong?
		 */
	case WM_DPICHANGED: {
		const RECT* rect = reinterpret_cast<RECT*>(lParam);
		SetWindowPos(hWnd, NULL, rect->left, rect->top,
			rect->right - rect->left,
			rect->bottom - rect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		return 0;
	}
#endif
#ifdef WM_GETDPISCALEDSIZE
	/*
	 * Windows gets this wrong (still as of Feb 2018), so we calculate it
	 * manually. The original size and scale work because 'hWnd' is still at
	 * the old DPI and 'wParam' is the new. Return TRUE to say we handled it
	 * ('lParam' contains the returned size).
	 * 
	 * TODO: grab the menu parameter
	 */
	case WM_GETDPISCALEDSIZE: {
		float scale = static_cast<float>(wParam) / getWindowDpi(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		rect.right  = static_cast<LONG>((rect.right - rect.left) * scale + 0.5f);
		rect.bottom = static_cast<LONG>((rect.bottom - rect.top) * scale + 0.5f);
		rect.left   = 0;
		rect.top    = 0;
		if (AdjustWindowRectExForDpi != NULL) {
			AdjustWindowRectExForDpi(&rect,
				GetWindowLong(hWnd, GWL_STYLE), FALSE,
				GetWindowLong(hWnd, GWL_EXSTYLE),
					static_cast<UINT>(wParam));
		} else {
			/*
			 * TODO: test this fallback on older Windows
			 */
			AdjustWindowRectEx(&rect,
				GetWindowLong(hWnd, GWL_STYLE), FALSE,
				GetWindowLong(hWnd, GWL_EXSTYLE));
		}
		SIZE* size = reinterpret_cast<SIZE*>(lParam);
		size->cx = rect.right - rect.left;
		size->cy = rect.bottom - rect.top;
		return TRUE;
	}
#endif
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/**
 * \return \c true if the application is still running (i.e. did not quit)
 */
bool yield() {
	bool running = true;
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			running = false;
		}
		TranslateMessage(&msg);
		DispatchMessage (&msg); 
	}
	Sleep(WINDOW_SLEEP_PERIOD);
	return running; 
}
}

//******************************** Public API ********************************/


inline static void print_glfw_error(int code, const char* _NULLABLE desc)
{
	printf("GLFW [%d]: %s", code, desc);
}

window::Handle window::create(unsigned winW, unsigned winH, const char* title) {
	glfwSetErrorCallback(print_glfw_error);
	if (!glfwInit())
		return NULLPTR;

	// Make sure GLFW does not initialize any graphics context.
	// This needs to be done explicitly later
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	_window = glfwCreateWindow((winW) ? winW : WINDOW_WIN_W, (winH) ? winH : WINDOW_WIN_H, (title) ? title : WINDOW_WIN_TITLE, NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		return NULLPTR;
	}

	HWND hwnd = glfwGetWin32Window(_window);
	return reinterpret_cast<window::Handle>(hwnd);
}


void window::destroy(window::Handle /*wHnd*/) {
	//DestroyWindow(TO_WIN(wHnd));
	glfwDestroyWindow(_window);
}

void window::show(window::Handle /*wHnd*/, bool show) {
	/*ShowWindow(TO_WIN(wHnd), (show) ? SW_SHOWDEFAULT : SW_HIDE);*/
	glfwShowWindow(_window);
}

void window::loop(window::Handle /*wHnd*/, window::Redraw func) {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		// render function callback
		func();
	}
}

/**
 * Converts mouse button constants from GLFW3 library into our library constants.
 */
int window::convertMouseButton(int button)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		return MOUSE_LEFT_BUTTON;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		return MOUSE_MIDDLE_BUTTON;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		return MOUSE_RIGHT_BUTTON;
	}

	return -1;
}

/**
 * Converts mouse action constants from GLFW3 library into our library constants.
 */
int window::convertMouseAction(int action)
{
	if (action == GLFW_PRESS) {
		return ACTION_PRESSED;
	}
	else if (action == GLFW_RELEASE) {
		return ACTION_RELEASED;
	}
	else if (action == GLFW_REPEAT) {
		return ACTION_REPEAT;
	}
	
	return -1;
}

/**
 * Binds mouse click inside the GLFW3 window to the Renderer mouse click handler.
 */
void window::mouseClicked(MouseHandler func)
{
	mouseClickHandlerClb = func;

	glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) {
		if (mouseClickHandlerClb != NULLPTR) {
			double xpos, ypos;
			//getting cursor position
			glfwGetCursorPos(window, &xpos, &ypos);

			mouseClickHandlerClb(convertMouseButton(button),
				convertMouseAction(action),
				(int)xpos,
				(int)ypos);
		}
	});
}

/**
 * Binds key press inside the GLFW3 window to the Renderer key press handler.
 */
void window::keyPressed(KeyHandler func)
{
	keyHandlerClb = func;

	glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (keyHandlerClb != NULLPTR) {
			keyHandlerClb(key, convertMouseAction(action));
		}
	});
}
