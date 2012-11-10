/* Arcan-fe, scriptable front-end engine
 *
 * Arcan-fe is the legal property of its developers, please refer
 * to the COPYRIGHT file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef _HAVE_ARCAN_EVENT
#define _HAVE_ARCAN_EVENT

#define ARCAN_JOYIDBASE 64
#define ARCAN_HATBTNBASE 128
#define ARCAN_MOUSEIDBASE 0

/* this is relevant if the event queue is authoritative,
 * i.e. the main process side with a frameserver associated. A failure to get
 * a lock within the set time, will forcibly free the frameserver */
#define DEFAULT_EVENT_TIMEOUT 500

enum ARCAN_EVENT_CATEGORY {
	EVENT_SYSTEM      = 1,
	EVENT_IO          = 2,
	EVENT_TIMER       = 4,
	EVENT_VIDEO       = 8,
	EVENT_AUDIO       = 16,
	EVENT_TARGET      = 32,
	EVENT_FRAMESERVER = 64,
	EVENT_EXTERNAL    = 128,
	EVENT_NET         = 256
};

enum ARCAN_EVENT_SYSTEM {
	EVENT_SYSTEM_EXIT = 0,
	EVENT_SYSTEM_VIDEO_FAIL,
	EVENT_SYSTEM_AUDIO_FAIL,
	EVENT_SYSTEM_IO_FAIL,
	EVENT_SYSTEM_MEMORY_FAIL,
	EVENT_SYSTEM_INACTIVATE,
	EVENT_SYSTEM_ACTIVATE,
	EVENT_SYSTEM_LAUNCH_EXTERNAL,
	EVENT_SYSTEM_CLEANUP_EXTERNAL,
	EVENT_SYSTEM_EVALCMD
};

enum ARCAN_TARGET_COMMAND {
/* notify that the child will be shut down / killed,
 * this happens in three steps (1) dms is released, (2) exit is enqueued, (3) sigterm is sent. */
	TARGET_COMMAND_EXIT,

/* notify that there is a file descriptor to be retrieved and set as the input/output fd for other
 * command events */
	TARGET_COMMAND_FDTRANSFER,

/* hinting event for frameskip modes (auto, process every n frames, singlestep) */
	TARGET_COMMAND_FRAMESKIP,
	TARGET_COMMAND_STEPFRAME,

/* hinting event for pushing state to the suggested file-descriptor */
	TARGET_COMMAND_STORE,

/* hinting event for restoring state from the suuggested file-descriptor */
	TARGET_COMMAND_RESTORE,

/* hinting event for reseting state to the first known initial steady one */
	TARGET_COMMAND_RESET,

/* hinting event for attempting to block the entire process until unpause is triggered */
	TARGET_COMMAND_PAUSE,
	TARGET_COMMAND_UNPAUSE,

/* plug in device of a specific kind in a set port */
	TARGET_COMMAND_SETIODEV,

/* specialized output hinting */
	TARGET_COMMAND_VECTOR_LINEWIDTH,
	TARGET_COMMAND_VECTOR_POINTSIZE,
	TARGET_COMMAND_NTSCFILTER,
	TARGET_COMMAND_NTSCFILTER_ARGS
};

enum ARCAN_EVENT_IO {
	EVENT_IO_BUTTON_PRESS, /*  joystick buttons, mouse buttons, ...    */
	EVENT_IO_BUTTON_RELEASE,
	EVENT_IO_KEYB_PRESS,
	EVENT_IO_KEYB_RELEASE,
	EVENT_IO_AXIS_MOVE
};

enum ARCAN_EVENT_IDEVKIND {
	EVENT_IDEVKIND_KEYBOARD,
	EVENT_IDEVKIND_MOUSE,
	EVENT_IDEVKIND_GAMEDEV
};

enum ARCAN_EVENT_IDATATYPE {
	EVENT_IDATATYPE_ANALOG,
	EVENT_IDATATYPE_DIGITAL,
	EVENT_IDATATYPE_TRANSLATED
};

enum ARCAN_EVENT_FRAMESERVER {
	EVENT_FRAMESERVER_RESIZED,
	EVENT_FRAMESERVER_TERMINATED,
#ifdef _DEBUG
	EVENT_FRAMESERVER_BUFFERSTATUS,
#endif
	EVENT_FRAMESERVER_LOOPED,
	EVENT_FRAMESERVER_VIDEOSOURCE_FOUND,
	EVENT_FRAMESERVER_VIDEOSOURCE_LOST,
	EVENT_FRAMESERVER_AUDIOSOURCE_FOUND,
	EVENT_FRAMESERVER_AUDIOSOURCE_LOST
};

enum ARCAN_EVENT_EXTERNAL {
	EVENT_EXTERNAL_NOTICE_MESSAGE,
	EVENT_EXTERNAL_NOTICE_FAILURE,
	EVENT_EXTERNAL_NOTICE_NEWFRAME,
	EVENT_EXTERNAL_NOTICE_STATESIZE,
};

enum ARCAN_EVENT_VIDEO {
	EVENT_VIDEO_EXPIRE = 0,
	EVENT_VIDEO_SCALED,
	EVENT_VIDEO_MOVED,
	EVENT_VIDEO_BLENDED,
	EVENT_VIDEO_ROTATED,
	EVENT_VIDEO_ASYNCHIMAGE_LOADED,
	EVENT_VIDEO_ASYNCHIMAGE_LOAD_FAILED
};

enum ARCAN_EVENT_NET {
	EVENT_NET_CONNECTED,
	EVENT_NET_DISCONNECTED,
	EVENT_NET_NORESPONSE,
	EVENT_NET_CUSTOMMSG,
	EVENT_NET_INPUTEVENT
};

enum ARCAN_EVENT_AUDIO {
	EVENT_AUDIO_PLAYBACK_FINISHED = 0,
	EVENT_AUDIO_PLAYBACK_ABORTED,
	EVENT_AUDIO_BUFFER_UNDERRUN,
	EVENT_AUDIO_PITCH_TRANSFORMATION_FINISHED,
	EVENT_AUDIO_GAIN_TRANSFORMATION_FINISHED,
	EVENT_AUDIO_OBJECT_GONE,
	EVENT_AUDIO_INVALID_OBJECT_REFERENCED
};

enum ARCAN_EVENTFILTER_ANALOG {
	EVENT_FILTER_ANALOG_NONE,
	EVENT_FILTER_ANALOG_ALL,
	EVENT_FILTER_ANALOG_SPECIFIC
};

enum ARCAN_TARGET_SKIPMODE {
	TARGET_SKIP_AUTO = 0,
	TARGET_SKIP_NONE = -1,
	TARGET_SKIP_STEP = 1
};

typedef union arcan_ioevent_data {
	struct {
		uint8_t devid;
		uint8_t subid;
		bool active;
	} digital;

	struct {
		bool gotrel; /* axis- values are first relative then absolute if set */
		uint8_t devid;
		uint8_t subid;
		uint8_t idcount;
		uint8_t nvalues;
		int16_t axisval[4];
	} analog;

	struct {
		bool active;
		uint8_t devid;
		uint16_t subid;

		uint16_t keysym;
		uint16_t modifiers;
		uint8_t scancode;
	} translated;

} arcan_ioevent_data;

typedef struct {
	enum ARCAN_EVENT_IDEVKIND devkind;
	enum ARCAN_EVENT_IDATATYPE datatype;

	arcan_ioevent_data input;
} arcan_ioevent;

typedef struct {
	arcan_vobj_id source;
	img_cons constraints;
	surface_properties props;
	intptr_t data;
} arcan_vevent;

typedef struct {
	arcan_vobj_id video;
	arcan_aobj_id audio;

	int width, height;

	unsigned c_abuffer, c_vbuffer;
	unsigned l_abuffer, l_vbuffer;

	bool glsource;

	intptr_t otag;
} arcan_fsrvevent;

typedef struct {
	arcan_aobj_id source;
	void* data;
} arcan_aevent;

typedef struct arcan_sevent {
	int errcode; /* copy of errno if possible */
	union {
		struct {
			long long hitag, lotag;
		} tagv;
		struct {
/* only for dev/dbg purposes, expected scripting frontend to free and not-mask */
			char* dyneval_msg;
		} mesg;
	} data;
} arcan_sevent;

typedef struct arcan_tevent {
	long long int pulse_count;
} arcan_tevent;

typedef struct arcan_netevent{
	arcan_vobj_id source;
	unsigned connid;

	union {
		char hostaddr[40]; /* max ipv6 textual representation, 39 */
		char connhandle[4];
	};
} arcan_netevent;

typedef struct arcan_tgtevent {
	enum ARCAN_TARGET_COMMAND command;
	union {
		int iv;
		float fv;
	} ioevs[4];
#ifdef _WIN32
	HANDLE fh;
#endif
} arcan_tgtevent;

typedef struct arcan_extevent {
	arcan_vobj_id source;

	union {
		char message[24];
		int32_t state_sz;
		uint32_t code;
		uint32_t framenumber;
	};
} arcan_extevent;

typedef union event_data {
	arcan_ioevent   io;
	arcan_vevent    video;
	arcan_aevent    audio;
	arcan_sevent    system;
	arcan_tevent    timer;
	arcan_tgtevent  target;
	arcan_fsrvevent frameserver;
	arcan_extevent  external;
	arcan_netevent  network;

	void* other;
} event_data;

typedef struct arcan_event {
	unsigned kind;
	unsigned tickstamp;

	char label[16];
	unsigned short category;

	event_data data;
} arcan_event;

struct arcan_evctx {
	bool interactive; /* should STDIN be processed for command events? */

	unsigned c_ticks;
	unsigned c_leaks;
	unsigned mask_cat_inp;
	unsigned mask_cat_out;

	unsigned kbdrepeat;

/* limit analog sampling rate as to not saturate the event buffer,
 * with rate == 0, all axis events will be emitted
 * with rate >  0, the upper limit is n samples per second.
 * with rate <  0, emit a sample per axis every n milliseconds. */
	struct {
		int rate;

/* with smooth samples > 0, use a ring-buffer of smooth_samples values per axis,
 * and whenever an sample is to be emitted, the actual value will be based on an average
 * of the smooth buffer. */
		char smooth_samples;
	} analog_filter;

	unsigned n_eventbuf;
	arcan_event* eventbuf;

	unsigned* front;
	unsigned* back;
	unsigned cell_aofs;

	bool local;
	union {
		void* local;

		struct {
			void* killswitch; /* assumed to be NULL or frameserver */
			sem_handle shared;
		} external;

	} synch;
};

typedef struct arcan_evctx arcan_evctx;

/* check timers, poll IO events and timing calculations
 * out : (NOT NULL) storage- container for number of ticks that has passed
 *                   since the last call to arcan_process
 * ret : range [0 > n < 1] how much time has passed towards the next tick */
float arcan_event_process(arcan_evctx*, unsigned* nticks);

/* check the queue for events,
 * don't attempt to run this one-
 * on more than one thread */
arcan_event* arcan_event_poll(arcan_evctx*);

/* similar to event poll, but will only
 * return events matching masked event */
arcan_event* arcan_event_poll_masked(arcan_evctx*, unsigned mask_cat, unsigned mask_ev);

/* force a keyboard repeat- rate */
void arcan_event_keyrepeat(arcan_evctx*, unsigned rate);

arcan_evctx* arcan_event_defaultctx();

/* Pushes as many events from srcqueue to dstqueue as possible without over-saturating.
 * allowed defines which kind of category that will be transferred, other events will be ignored.
 * The saturation cap is defined in 0..1 range as % of full capacity
 * specifying a source ID (can be ARCAN_EID) will be used for rewrites if the category has a source identifier
 */
void arcan_event_queuetransfer(arcan_evctx* dstqueue, arcan_evctx* srcqueue,
	enum ARCAN_EVENT_CATEGORY allowed, float saturation, arcan_vobj_id source);

/* enqueue an event into the specified event-context,
 * for non-frameserver part of the application, this can be substituted with arcan_event_defaultctx()
 * thread-safe */
void arcan_event_enqueue(arcan_evctx*, const arcan_event*);

/* ignore-all on enqueue */
void arcan_event_maskall(arcan_evctx*);

/* drop any mask, including maskall */
void arcan_event_clearmask(arcan_evctx*);

/* set a specific mask, somewhat limited */
void arcan_event_setmask(arcan_evctx*, unsigned mask);

int64_t arcan_frametime();

/*
 * special case, due to the event driven approach of LUA invocation,
 * we can get situations where we have a queue of events related to a certain vid/aid,
 * after the user has explicitly asked for it to be deleted.
 *
 * This means the user either has to check for this condition by tracking the object (possibly dangling references etc.)
 * or that we sweep the queue and erase the tracks of the object in question.
 *
 * the default behaviour is to not erase unprocessed events that are made irrelevant due to a deleted object.
 */
void arcan_event_erase_vobj(arcan_evctx* ctx, enum ARCAN_EVENT_CATEGORY category, arcan_vobj_id source);

void arcan_event_init(arcan_evctx* dstcontext);
void arcan_event_deinit(arcan_evctx*);

/* call to dump the contents of the queue */
void arcan_event_dumpqueue(arcan_evctx*);

/*
 * Supply a buffer sizeof(arcan_event) or larger and it'll be packed down to an internal format (XDR) for serialization,
 * which can be transmitted over the wire and later deserialized again with unpack.
 * dbuf is dynamically allocated, and the payload is padded by 'pad' byte, final size is stored in sz.
 * returns false on out of memory or bad in event
 */
bool arcan_event_pack(arcan_event*, int pad, char** dbuf, size_t* sz);

/* takes an input character buffer, unpacks it and stores the result in dst.
 * returns false if any of the arguments are missing or the buffer contents is invalid */
arcan_event arcan_event_unpack(arcan_event* dst, char* buf, size_t* bufsz);
#endif
