/*
 * Copyright 2014-2015, Björn Ståhl
 * License: 3-Clause BSD, see COPYING file in arcan source repository.
 * Reference: http://arcan-fe.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "../video_platform.h"
#include "../platform.h"

#include "arcan_math.h"
#include "arcan_general.h"
#include "arcan_video.h"
#include "arcan_videoint.h"

#ifdef GLES3
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void glReadBuffer(GLenum mode)
{
}

void glWriteBuffer(GLenum mode)
{
}

#endif

static const char* defvprg =
"#version 100\n"
"precision mediump float;\n"
"uniform mat4 modelview;\n"
"uniform mat4 projection;\n"

"attribute vec2 texcoord;\n"
"varying vec2 texco;\n"
"attribute vec4 vertex;\n"
"void main(){\n"
"	gl_Position = (projection * modelview) * vertex;\n"
"   texco = texcoord;\n"
"}";

const char* deffprg =
"#version 100\n"
"precision mediump float;\n"
"uniform sampler2D map_diffuse;\n"
"varying vec2 texco;\n"
"uniform float obj_opacity;\n"
"void main(){\n"
"   vec4 col = texture2D(map_diffuse, texco);\n"
"   col.a = col.a * obj_opacity;\n"
"	gl_FragColor = col;\n"
"}";

const char* defcfprg =
"#version 100\n"
"precision mediump float;\n"
"varying vec2 texco;\n"
"uniform vec3 obj_col;\n"
"uniform float obj_opacity;\n"
"void main(){\n"
"   gl_FragColor = vec4(obj_col.rgb, obj_opacity);\n"
"}\n";

const char * defcvprg =
"#version 100\n"
"precision mediump float;\n"
"uniform mat4 modelview;\n"
"uniform mat4 projection;\n"
"attribute vec4 vertex;\n"
"void main(){\n"
" gl_Position = (projection * modelview) * vertex;\n"
"}";

arcan_shader_id agp_default_shader(enum SHADER_TYPES type)
{
	static arcan_shader_id shids[SHADER_TYPE_ENDM];
	static bool defshdr_build;

	assert(type < SHADER_TYPE_ENDM);

	if (!defshdr_build){
		shids[0] = arcan_shader_build("DEFAULT", NULL, defvprg, deffprg);
		shids[1] = arcan_shader_build("DEFAULT_COLOR", NULL, defcvprg, defcfprg);
		defshdr_build = true;
	}

	return shids[type];
}

const char** agp_envopts()
{
	static const char* env[] = {
		NULL
	};
	return env;
}

const char* agp_shader_language()
{
	return "GLSL100";
}

const char* agp_ident()
{
#ifdef GLES3
	return "GLES3";
#else
	return "GLES2";
#endif
}

void agp_shader_source(enum SHADER_TYPES type,
	const char** vert, const char** frag)
{
	switch(type){
		case BASIC_2D:
			*vert = defvprg;
			*frag = deffprg;
		break;

		case COLOR_2D:
			*vert = defcvprg;
			*frag = defcfprg;
		break;

		default:
			*vert = NULL;
			*frag = NULL;
		break;
	}
}

void agp_env_help(FILE* out)
{
/* write agp specific tuning here,
 * use ARCAN_AGP_ as symbol prefix */
}

void glDrawBuffer(GLint mode)
{
}

/*
 * NOTE: not yet implemented, there's few "clean" ways for
 * getting this behavior in GLES2,3 -- either look for an
 * extension that permits it or create an intermediate
 * rendertarget, blit to it, and then glReadPixels (seriously...)
 *
 * Possibly use a pool of a few stores at least as
 * the amount of readbacks should be rather limited
 * (we're talking recording, streaming or remoting)
 * something like:
 *
 *	glGenRenderbuffers(1, &readback.texid);
 *  glBindRenderbuffer(GL_RENDERBUFFER, readback.texid);
 *	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
 *
 *	glGenFramebuffers(1, &readback.fboid);
 *	glBindFramebuffer(GL_FRAMEBUFFER, readback.fboid);
 *	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
 *		GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, readback.texid);
 *
 *	glViewport(0, 0, width, height);
 *	glReadPixels(0, 0, width, height, GL_RGBA, GL_RGBA, dstbuf);
 */

void agp_readback_synchronous(struct storage_info_t* dst)
{
}

void agp_request_readback(struct storage_info_t* store)
{
}

struct asynch_readback_meta argp_buffer_readback_asynchronous(
	struct storage_info_t* dst, bool poll)
{
	struct asynch_readback_meta res = {0};
	return res;
}

static void default_release(void* tag)
{
}

struct asynch_readback_meta agp_poll_readback(struct storage_info_t* store)
{
	struct asynch_readback_meta res = {
	.release = default_release
	};

	return res;
}

void agp_drop_vstore(struct storage_info_t* s)
{
	if (!s)
		return;

	glDeleteTextures(1, &s->vinf.text.glid);
	s->vinf.text.glid = 0;

#ifdef GLES3
	if (GL_NONE != s->vinf.text.rid)
		glDeleteBuffers(1, &s->vinf.text.rid);
#endif

	if (GL_NONE != s->vinf.text.wid)
		glDeleteBuffers(1, &s->vinf.text.wid);

	memset(s, '\0', sizeof(struct storage_info_t));
}

void agp_resize_vstore(struct storage_info_t* s, size_t w, size_t h)
{
	s->w = w;
	s->h = h;
	s->bpp = GL_PIXEL_BPP;

	if (s->vinf.text.raw){
		arcan_mem_free(s->vinf.text.raw);
		s->vinf.text.s_raw = w * h * s->bpp;
		s->vinf.text.raw = arcan_alloc_mem(s->vinf.text.s_raw,
			ARCAN_MEM_VBUFFER, ARCAN_MEM_BZERO, ARCAN_MEMALIGN_PAGE);
	}

	agp_update_vstore(s, true);
}

struct stream_meta agp_stream_prepare(struct storage_info_t* s,
		struct stream_meta meta, enum stream_type type)
{
	struct stream_meta mout = meta;
	mout.state = true;

	switch(type){
	case STREAM_RAW:
		if (!s->vinf.text.raw){
			s->vinf.text.s_raw = s->w * s->h * GL_PIXEL_BPP;
			s->vinf.text.raw = arcan_alloc_mem(s->vinf.text.s_raw,
				ARCAN_MEM_VBUFFER, ARCAN_MEM_BZERO, ARCAN_MEMALIGN_PAGE);
		}
		mout.buf = s->vinf.text.raw;
		mout.state = mout.buf != NULL;
	break;

	case STREAM_RAW_DIRECT:
	case STREAM_RAW_DIRECT_SYNCHRONOUS:
		agp_activate_vstore(s);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->w, s->h,
			GL_PIXEL_FORMAT, GL_UNSIGNED_BYTE, meta.buf);
		agp_deactivate_vstore();

	case STREAM_HANDLE:
		mout.state = platform_video_map_handle(
			s, meta.handle); /* see notes in gl21.c */
	break;
	}

	return mout;
}

void agp_stream_release(struct storage_info_t* s)
{
}

void agp_stream_commit(struct storage_info_t* s)
{
	agp_activate_vstore(s);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->w, s->h,
			GL_PIXEL_FORMAT, GL_UNSIGNED_BYTE, s->vinf.text.raw);
	agp_deactivate_vstore(s);
}
