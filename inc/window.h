/**
 * \file window.h
 * Abstraction for creating and managing windows.
 */
#pragma once

#include "defines.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

namespace window {
/**
 * \typedef Handle
 * Opaque window handle.
 */
typedef struct HandleImpl* Handle;

/**
 * Function prototype for the redraw callback. See \c #loop().
 */
typedef bool (*Redraw) ();

typedef void (*MouseHandler) (int, int, int, int);
typedef void (*KeyHandler) (int, int);
typedef void (*ResizeHandler) (int, int);

//****************************************************************************/



/**
 * Creates a new window.
 *
 * \param[in] winW optional internal width (or zero to use the default width)
 * \param[in] winH optional internal height (or zero to use the default height)
 * \param[in] title optional window title (alternatively repurposed as the element ID for web-based implementations)
 */
Handle _NULLABLE create(unsigned winW = 0, unsigned winH = 0, const char* _NULLABLE title = NULLPTR);



/**
 * Destroys a window, freeing any resources.
 *
 * \param[in] wHnd window to destroy
 */
void destroy(Handle _NONNULL wHnd);

/**
 * Shows or hides a window.
 *
 * \param[in] wHnd window to show or hide
 * \param[in] show \c true to show, \c false to hide
 */
void show(Handle _NONNULL wHnd, bool show = true);

/**
 * Registers the redraw function to be called each frame.
 *
 * \note Currently this blocks, returning only when the redraw function returns.
 *
 * \todo rethink this - what do we do for multiple windows?
 *
 * \param[in] wHnd window to synchronise the redraw with
 * \param[in] func function to be called each \e frame (or \c null to do nothing)
 */
void loop(Handle _NONNULL wHnd, Redraw _NULLABLE func = NULLPTR);


static MouseHandler _NULLABLE mouseClickHandlerClb;
static ResizeHandler _NULLABLE resizeHandlerClb;
static KeyHandler _NULLABLE keyHandlerClb;


// These are methods for converting library-specific constants into our constants.	
static int convertMouseButton(int button);
static int convertMouseAction(int action);

inline static void print_glfw_error(int code, const char* _NULLABLE desc)
{
	printf("GLFW [%d]: %s", code, desc);
}

void mouseClicked(MouseHandler _NULLABLE func = NULLPTR);
void keyPressed(KeyHandler _NULLABLE func = NULLPTR);

}
