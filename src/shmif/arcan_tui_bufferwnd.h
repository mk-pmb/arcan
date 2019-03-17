/*
 Arcan Text-Oriented User Inteface Library, Extensions
 Copyright: 2018-2019, Bjorn Stahl
 License: 3-clause BSD
 Description: This header describes optional support components that
 extend TUI with some common helpers for input. They also server as
 simple examples for how to build similar ones, to lift, patch and
 include in custom projects.
*/

#ifndef HAVE_TUI_EXT_BUFFERWND
#define HAVE_TUI_EXT_BUFFERWND

/*
 * Description:
 * This function partially assumes control over a provided window and uses
 * it to present a view into the contents of the provided buffer. It takes
 * care of rendering, layouting, cursor management and text/binary working
 * modes.
 *
 * The caller is expected to run the normal tui refresh event loop.
 *
 * Arguments:
 * [buf] describes the buffer that will be exposed in the window.
 * [opts | NULL] contains statically controlled settings, versioned
 * with the size of the structure.
 *
 * Handlers/Allocation:
 * This dynamically allocates internally, and replaces the the normal set
 * of handlers. Use _bufferwnd_release to return the context to the state
 * it was before this function was called.
 *
 * The following list of functions will be chained and forwarded:
 * query_label
 * input_label
 *
 * Example:
 * see the #ifdef EXAMPLE block at the bottom of tui_bufferwnd.c
 */

enum bufferwnd_display_modes {
	BUFFERWND_VIEW_ASCII = 0,
	BUFFERWND_VIEW_UTF8 = 1,
	BUFFERWND_VIEW_HEX = 2,
	BUFFERWND_VIEW_HEX_DETAIL = 3
};

enum bufferwnd_wrap_mode {
	BUFFERWND_WRAP_ALL = 0,
	BUFFERWND_WRAP_ACCEPT_LF,
	BUFFERWND_WRAP_ACCEPT_CR_LF
};

enum bufferwnd_color_mode {
	BUFFERWND_COLOR_NONE = 0,
	BUFFERWND_COLOR_PALETTE = 1,
	BUFFERWND_COLOR_CUSTOM = 2
};

/* hook to allow custom (data-dependent) colorization for
 * [bytev] at buffer position [pos],
 * write value into [fg] and [bg] */
typedef void(*color_lookup_fn)(struct tui_context* T, void* tag,
	uint8_t bytev, size_t pos, uint8_t fg[3], uint8_t bg[3]);

typedef bool(*commit_write_fn)(struct tui_context* T,
	void* tag, const uint8_t* buf, size_t nb, size_t ofs);

struct tui_bufferwnd_opts {
/* Disable any editing controls */
	bool read_only;

/* All cursor management (moving, selection, ...) is disabled */
	bool hide_cursor;

/* Initial display/wrap_mode, this is still user-changeable */
	int view_mode;
	int wrap_mode;
	int color_mode;

/* Hooks for custom colorization, and a validation / commit function for
 * buffer edits */
	color_lookup_fn custom_color;
	commit_write_fn commit;
	void* cbtag;

/* When a buffer position is written out, apply this offset value first */
	uint64_t offset;
};

void arcan_tui_bufferwnd_setup(
	struct tui_context* ctx, uint8_t* buf, size_t buf_sz,
	struct tui_bufferwnd_opts*, size_t opts_sz
);

/*
 * Force- navigate the window to a specific position in the buffer and at
 * a specific offset within the window of that buffer.
 *
 * options:
 *  BUFFER_SEEK_FIXED : Ignore position, only seek to offset within current
 *                      window. Overflow / underflow will be ignored.
 */
void arcan_tui_bufferwnd_seek(
	struct tui_context* ctx, size_t pos, size_t ofs, int seek_opt);

/*
 * Take a context that has previously been setup via arcan_tui_bufferwnd_setup,
 * and restore its set of handlers/tag (not the contents itself) to the state
 * it was on the initial call. Normal deallocation might happen as part of the
 * on_destroy event handler on the other hand, and in such cases _free should
 * not be called.
 */
void arcan_tui_bufferwnd_free(struct tui_context*);
#endif
