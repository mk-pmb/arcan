/* Arcan-fe (OS/device platform), scriptable front-end engine
 *
 * Arcan-fe is the legal property of its developers, please refer
 * to the platform/LICENSE file distributed with this source distribution
 * for licensing terms.
 */

/*
 * PLATFORM DRIVER NOTICE:
 * This platform driver is incomplete in the sense that it was only set
 * up in order to allow for headless LWA/hijack/retro3d.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "arcan_math.h"
#include "arcan_general.h"
#include "arcan_video.h"
#include "arcan_videoint.h"

#ifndef PLATFORM_SUFFIX
#define PLATFORM_SUFFIX platform
#endif

#define MERGE(X,Y) X ## Y
#define EVAL(X,Y) MERGE(X,Y)
#define PLATFORM_SYMBOL(fun) EVAL(PLATFORM_SUFFIX, fun)

#include <OpenGL/OpenGL.h>
#include <OpenGL/GL.h>

static char* x11_synchopts[] = {
	"default", "driver- specific GL swap",
	NULL
};

static CGLContextObj context;

bool PLATFORM_SYMBOL(_video_init)(uint16_t w, uint16_t h,
	uint8_t bpp, bool fs, bool frames, const char* cap)
{
	CGLPixelFormatObj pix;
	CGLError errorCode;
	GLint num;

	CGLPixelFormatAttribute attributes[4] = {
  	kCGLPFAAccelerated,
  	kCGLPFAOpenGLProfile,
  	(CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
  	(CGLPixelFormatAttribute) 0
	};

	errorCode = CGLChoosePixelFormat( attributes, &pix, &num );
  errorCode = CGLCreateContext( pix, NULL, &context );
	CGLDestroyPixelFormat( pix );
  errorCode = CGLSetCurrentContext( context );

/*
 * no double buffering,
 * we let the parent transfer process act as the limiting clock.
 */
	GLint si = 0;
	CGLSetParameter(context, kCGLCPSwapInterval, &si);

#ifndef HEADLESS_NOARCAN
	arcan_video_display.width = w;
	arcan_video_display.height = h;
#endif

	return true;
}

void PLATFORM_SYMBOL(_video_synch)(uint64_t tick_count, float fract,
	video_synchevent pre, video_synchevent post)
{
	if (pre)
		pre();

#ifndef HEADLESS_NOARCAN
	size_t nd;
	arcan_bench_register_cost( arcan_vint_refresh(fract, &nd) );

	agp_activate_rendertarget(NULL);

	if (nd > 0){
		arcan_vint_drawrt(arcan_vint_world(), 0, 0,
			arcan_video_display.width, arcan_video_display.height
		);
	}

	arcan_vint_drawcursor(true);
	arcan_vint_drawcursor(false);
#endif

	glFlush();

	if (post)
		post();
}

const char** PLATFORM_SYMBOL(_video_synchopts)(void)
{
	return (const char**) x11_synchopts;
}

void PLATFORM_SYMBOL(_video_setsynch)(const char* arg)
{
	arcan_warning("unhandled synchronization strategy (%s) ignored.\n", arg);
}

void PLATFORM_SYMBOL(_video_prepare_external) () {}
void PLATFORM_SYMBOL(_video_restore_external) () {}

void PLATFORM_SYMBOL(_video_shutdown) ()
{
}

void* PLATFORM_SYMBOL(_video_gfxsym)(const char* sym)
{
	return NULL;
}

const char* PLATFORM_SYMBOL(_video_capstr)(void)
{
	static char* capstr;

	if (!capstr){
		const char* vendor = (const char*) glGetString(GL_VENDOR);
		const char* render = (const char*) glGetString(GL_RENDERER);
		const char* version = (const char*) glGetString(GL_VERSION);
		const char* shading = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		const char* exts = (const char*) glGetString(GL_EXTENSIONS);

		size_t interim_sz = 64 * 1024;
		char* interim = malloc(interim_sz);
		size_t nw = snprintf(interim, interim_sz, "Video Platform (LWA-Darwin)\n"
			"Vendor: %s\nRenderer: %s\nGL Version: %s\n"
			"GLSL Version: %s\n\n Extensions Supported: \n%s\n\n",
			vendor, render, version, shading, exts
		) + 1;

		if (nw < (interim_sz >> 1)){
			capstr = malloc(nw);
			memcpy(capstr, interim, nw);
			free(interim);
		}
		else
			capstr = interim;
	}

	return capstr;
}

void PLATFORM_SYMBOL(_video_minimize) () {}

