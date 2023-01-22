#include "window.h"

#import <CoreVideo/CoreVideo.h>

#include "glue.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <stdio.h>
GLFWwindow* _NULLABLE _window;

/**
 * Default window width.
 */
#ifndef WINDOW_WIN_W
#define WINDOW_WIN_W 800
#endif

/**
 * Default window height.
 */
#ifndef WINDOW_WIN_H
#define WINDOW_WIN_H 600
#endif

/**
 * Default window title.
 */
#ifndef WINDOW_WIN_TITLE
#define WINDOW_WIN_TITLE "CS248"
#endif

/**
 * Set if the window should be resizable (otherwise it is fixed to the width
 * and height defined above).
 *
 * \todo disable arbitrary resize and support fixed options?
 */
#ifndef WINDOW_WIN_RESIZE
#define WINDOW_WIN_RESIZE 0
#endif

/*
 * Sleep period for the yield. Currently undergoing experimentation (the vblank
 * wait should do away with this requirement).
 */
#ifndef WINDOW_SLEEP_PERIOD
#define WINDOW_SLEEP_PERIOD 2
#endif

/**
 * Window style flags passed to window creation call.
 */
#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101200
#if (WINDOW_WIN_RESIZE)
#define WINDOW_NS_WIN_FLAGS NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
#else
#define WINDOW_NS_WIN_FLAGS NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
#endif
#else
#if (WINDOW_WIN_RESIZE)
#define WINDOW_NS_WIN_FLAGS NSTitledWindowMask | NSClosableWindowMask| NSMiniaturizableWindowMask | NSResizableWindowMask
#else
#define WINDOW_NS_WIN_FLAGS NSTitledWindowMask | NSClosableWindowMask| NSMiniaturizableWindowMask
#endif
#endif

//****************************************************************************/

/**
 * Helper to convert a \c Handle to a window.
 */
#define TO_HND(win) reinterpret_cast<window::Handle>(win)
/**
 * Helper to convert a window to a \c Handle.
 */
#define TO_WIN(hnd) reinterpret_cast<WindowImpl*>(hnd)

namespace impl {
/**
 * \c true if the application is still running.
 *
 * \todo the currenty only works for a single window
 */
bool running = true;

/**
 * Display link callback (with the \c CVDisplayLinkOutputCallback signature).
 *
 * \param[in] dispLink display link requesting the frame
 * \param[in] callTime time now of the current call
 * \param[in] drawTime time the frame will be displayed
 * \param[in] user user defined data (currently the owning \c WindowImpl pointer)
 */
static CVReturn update(CVDisplayLinkRef dispLink, const CVTimeStamp* callTime, const CVTimeStamp* drawTime, CVOptionFlags, CVOptionFlags*, void* user);
}

#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
@interface WindowImpl : NSWindow<NSWindowDelegate, NSDraggingDestination>
#else
@interface WindowImpl : NSWindow
#endif
{
/**
 * Display link compatible with all displays.
 *
 * \todo assign to the window's display (does this guarantee the correct refresh on multi-monitor systems?)
 * \todo track the window moving monitors
 */
CVDisplayLinkRef dispLink;

/**
 * Registered redraw function to call each frame via \c #dispLink.
 */
window::Redraw redrawFunc;
}
@end

/**
 * Single application window.
 */
@implementation WindowImpl
/**
 * \copydoc NSWindow#initWithContentRect
 */
- (id)initWithContentRect:(NSRect)rect styleMask:(NSUInteger)style backing:(NSBackingStoreType)type defer:(BOOL)flag {
	if ((self = [super initWithContentRect:rect styleMask:style backing:type defer:flag])) {
		CVDisplayLinkCreateWithActiveCGDisplays(&dispLink);
		CVDisplayLinkSetOutputCallback(dispLink, &impl::update, self);
		redrawFunc = NULLPTR;
		/*
		 * Note: added as a notification instead of an NSWindowDelegate,
		 * allowing other parts of metal to add their own.
		 */
		[[NSNotificationCenter defaultCenter] addObserver:self
			selector:@selector(windowWillClose:)
				name:NSWindowWillCloseNotification object:self];
	}
	return self;
}

/**
 * \copydoc NSWindow#dealloc
 */
- (void)dealloc {
	CVDisplayLinkRelease(dispLink);
	[super dealloc];
}

/**
 * Hmm, hackily sets the redraw function called by the display link.
 *
 * \todo tidy!
 * \todo this is freezing sometime on calling CVDisplayLinkStop (reproducible more in debug and with the Big Sur beta)
 *
 * \param[in] func redraw function (\c null to stop the redraw callbacks)
 */
- (void)setRedraw:(window::Redraw) func {
	if (func) {
		redrawFunc = func;
		CVDisplayLinkStart(dispLink);
	} else {
		impl::running = false;
		redrawFunc = NULLPTR;
		CVDisplayLinkStop(dispLink);
	}
}

/**
 * Called by the \c impl#update() redirector to call and handle \c #redrawFunc.
 */
- (void)doRedraw {
AUTO_RELEASE_POOL_ACQUIRE;
	if (!(redrawFunc && redrawFunc())) {
		[self setRedraw:NULLPTR];
	}
AUTO_RELEASE_POOL_RELEASE;
}

/**
 * Called when the window is about to close.
 *
 * \note Added via \c NSNotificationCenter not as a \c NSWindowDelegate.
 * \todo maintain a list of windows and don't just expect this to be the only one
 */
- (void)windowWillClose:(NSNotification*)__unused notification {
	/*
	 * The user closed the window so we kill the app (by removing its display
	 * link callback, which then stop the global 'running', eventually exiting
	 * the yield).
	 */
	[self setRedraw:NULLPTR];
	
}
@end

namespace impl {
static CVReturn update(CVDisplayLinkRef dispLink, const CVTimeStamp* callTime, const CVTimeStamp* drawTime, CVOptionFlags, CVOptionFlags*, void* user) {
	[TO_WIN(user)
		performSelectorOnMainThread:@selector(doRedraw)
			withObject:nil waitUntilDone:YES];
	/*
	NSLog(@"callTime:%lldms, drawTime:%lldms",
		  callTime->videoTime / (callTime->videoTimeScale / 1000),
		  drawTime->videoTime / (drawTime->videoTimeScale / 1000));
	 */
	return kCVReturnSuccess;
}

/*
 * Blocks waiting for events (processing each). Eventually exiting when \c
 * #running is \c false.
 *
 * \note This seems to run as expected, with 0.1% CPU as long as the display
 * link callback doesn't draw anything.
 */
void wait() {
AUTO_RELEASE_POOL_ACQUIRE;
	/*
	 * TODO: do we need the [NSApp updateWindows] call after each event?
	 */
	while (impl::running) {
		NSEvent* e = [NSApp nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantFuture]
				inMode:NSDefaultRunLoopMode dequeue:YES];
		if (e) {
			switch ([e type]) {
			/*
			 * Any of the events we're interested in go here.
			 */
			default:
				break;
			}
			[NSApp sendEvent:e];
			[NSApp updateWindows];
		}
	}
AUTO_RELEASE_POOL_RELEASE;
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

	WindowImpl* win = glfwGetCocoaWindow(_window);
	return reinterpret_cast<window::Handle>(win);
}
void window::destroy(window::Handle wHnd) {
	//[TO_WIN(wHnd) close];
	glfwDestroyWindow(_window);
}

void window::show(window::Handle /*wHnd*/, bool /*show*/) {
	glfwShowWindow(_window);
}

void window::loop(window::Handle wHnd, window::Redraw func) {
	/*
	 * Starts with an initial call to the draw function (which, for example,
	 * clears the screen early).
	 */
	while(!(glfwWindowShouldClose(_window)))
	{
		glfwPollEvents();
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