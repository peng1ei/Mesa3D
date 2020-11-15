/*
	MesaGL32.h
*/


#ifndef MESAGL32_H
#define MESAGL32_H


#ifdef __cplusplus
extern "C" {
#endif


#include <windows.h>
#include "\mesa127\include\gl\gl.h"


/*
 * This is the WMesa context 'handle':
 */
typedef struct wmesa_context *WMesaContext;

/*
*/
#ifdef MESAGL32_C
#define	DLLEXPORT
#else
#define DLLEXPORT
#endif

/*
 * Create a new WMesaContext for rendering into a window.  You must
 * have already created the window of correct visual type and with an
 * appropriate colormap.
 *
 * Input:
 *         hWnd - Window handle
 *         Pal  - Palette to use
 *         rgb_flag - GL_TRUE = RGB mode,
 *                    GL_FALSE = color index mode
 *         db_flag - GL_TRUE = double-buffered,
 *                   GL_FALSE = single buffered
 *
 * Note: Indexed mode requires double buffering under Windows.
 *
 * Return:  a WMesa_context or NULL if error.
 */
DLLEXPORT APIENTRY WMesaContext WMesaCreateContext(HWND hWnd,HPALETTE Pal,
                                       GLboolean rgb_flag,GLboolean db_flag);


/*
 * Destroy a rendering context as returned by WMesaCreateContext()
 */
DLLEXPORT APIENTRY void WMesaDestroyContext( WMesaContext ctx );


/*
 * Make the specified context the current one.
 */
DLLEXPORT APIENTRY void WMesaMakeCurrent( WMesaContext ctx );


/*
 * Return a handle to the current context.
 */
DLLEXPORT APIENTRY WMesaContext WMesaGetCurrentContext( void );


/*
 * Swap the front and back buffers for the current context.  No action
 * taken if the context is not double buffered.
 */
DLLEXPORT APIENTRY void WMesaSwapBuffers(void);


/*
 * In indexed color mode we need to know when the palette changes.
 */
DLLEXPORT APIENTRY void WMesaPaletteChange(HPALETTE Pal);



#ifdef __cplusplus
}
#endif


#endif

