#pragma once
#include "PublicInclude.h"
#include "UnExport.h"

//读取输入文件协议的时候使用；来自ffmpeg源码
typedef struct URLContext {
	const AVClass* av_class; ///< information for av_log(). Set by url_open().
	struct URLProtocol* prot;
	int flags;
	int is_streamed;  /**< true if streamed (no seek possible), default = false */
	int max_packet_size;  /**< if non zero, the stream is packetized with this max packet size */
	void* priv_data;
	char* filename; /**< specified URL */
	int is_connected;
	AVIOInterruptCB interrupt_callback;
} URLContext;
typedef struct URLProtocol {
	const char* name;
	int (*url_open)(URLContext* h, const char* url, int flags);
	int (*url_read)(URLContext* h, unsigned char* buf, int size);
	int (*url_write)(URLContext* h, const unsigned char* buf, int size);
	int64_t(*url_seek)(URLContext* h, int64_t pos, int whence);
	int (*url_close)(URLContext* h);
	struct URLProtocol* next;
	int (*url_read_pause)(URLContext* h, int pause);
	int64_t(*url_read_seek)(URLContext* h, int stream_index,
		int64_t timestamp, int flags);
	int (*url_get_file_handle)(URLContext* h);
	int priv_data_size;
	const AVClass* priv_data_class;
	int flags;
	int (*url_check)(URLContext* h, int mask);
} URLProtocol;
//---------------------

enum AVChannel {
	///< Invalid channel index
	AV_CHAN_NONE = -1,
	AV_CHAN_FRONT_LEFT,
	AV_CHAN_FRONT_RIGHT,
	AV_CHAN_FRONT_CENTER,
	AV_CHAN_LOW_FREQUENCY,
	AV_CHAN_BACK_LEFT,
	AV_CHAN_BACK_RIGHT,
	AV_CHAN_FRONT_LEFT_OF_CENTER,
	AV_CHAN_FRONT_RIGHT_OF_CENTER,
	AV_CHAN_BACK_CENTER,
	AV_CHAN_SIDE_LEFT,
	AV_CHAN_SIDE_RIGHT,
	AV_CHAN_TOP_CENTER,
	AV_CHAN_TOP_FRONT_LEFT,
	AV_CHAN_TOP_FRONT_CENTER,
	AV_CHAN_TOP_FRONT_RIGHT,
	AV_CHAN_TOP_BACK_LEFT,
	AV_CHAN_TOP_BACK_CENTER,
	AV_CHAN_TOP_BACK_RIGHT,
	/** Stereo downmix. */
	AV_CHAN_STEREO_LEFT = 29,
	/** See above. */
	AV_CHAN_STEREO_RIGHT,
	AV_CHAN_WIDE_LEFT,
	AV_CHAN_WIDE_RIGHT,
	AV_CHAN_SURROUND_DIRECT_LEFT,
	AV_CHAN_SURROUND_DIRECT_RIGHT,
	AV_CHAN_LOW_FREQUENCY_2,
	AV_CHAN_TOP_SIDE_LEFT,
	AV_CHAN_TOP_SIDE_RIGHT,
	AV_CHAN_BOTTOM_FRONT_CENTER,
	AV_CHAN_BOTTOM_FRONT_LEFT,
	AV_CHAN_BOTTOM_FRONT_RIGHT,


	AV_CHAN_UNUSED = 0x200,


	AV_CHAN_UNKNOWN = 0x300,
	AV_CHAN_AMBISONIC_BASE = 0x400,
	AV_CHAN_AMBISONIC_END = 0x7ff,
};

enum AVChannelOrder {
	AV_CHANNEL_ORDER_UNSPEC,
	AV_CHANNEL_ORDER_NATIVE,
	AV_CHANNEL_ORDER_CUSTOM,
	AV_CHANNEL_ORDER_AMBISONIC,
};

typedef struct AVChannelCustom {
	enum AVChannel id;
	char name[16];
	void* opaque;
} AVChannelCustom;

typedef struct AVChannelLayout {
	/**
	 * Channel order used in this layout.
	 * This is a mandatory field.
	 */
	enum AVChannelOrder order;

	/**
	 * Number of channels in this layout. Mandatory field.
	 */
	int nb_channels;

	/**
	 * Details about which channels are present in this layout.
	 * For AV_CHANNEL_ORDER_UNSPEC, this field is undefined and must not be
	 * used.
	 */
	union {
		/**
		 * This member must be used for AV_CHANNEL_ORDER_NATIVE, and may be used
		 * for AV_CHANNEL_ORDER_AMBISONIC to signal non-diegetic channels.
		 * It is a bitmask, where the position of each set bit means that the
		 * AVChannel with the corresponding value is present.
		 *
		 * I.e. when (mask & (1 << AV_CHAN_FOO)) is non-zero, then AV_CHAN_FOO
		 * is present in the layout. Otherwise it is not present.
		 *
		 * @note when a channel layout using a bitmask is constructed or
		 * modified manually (i.e.  not using any of the av_channel_layout_*
		 * functions), the code doing it must ensure that the number of set bits
		 * is equal to nb_channels.
		 */
		uint64_t mask;
		/**
		 * This member must be used when the channel order is
		 * AV_CHANNEL_ORDER_CUSTOM. It is a nb_channels-sized array, with each
		 * element signalling the presence of the AVChannel with the
		 * corresponding value in map[i].id.
		 *
		 * I.e. when map[i].id is equal to AV_CHAN_FOO, then AV_CH_FOO is the
		 * i-th channel in the audio data.
		 *
		 * When map[i].id is in the range between AV_CHAN_AMBISONIC_BASE and
		 * AV_CHAN_AMBISONIC_END (inclusive), the channel contains an ambisonic
		 * component with ACN index (as defined above)
		 * n = map[i].id - AV_CHAN_AMBISONIC_BASE.
		 *
		 * map[i].name may be filled with a 0-terminated string, in which case
		 * it will be used for the purpose of identifying the channel with the
		 * convenience functions below. Otherise it must be zeroed.
		 */
		AVChannelCustom* map;
	} u;

	/**
	 * For some private data of the user.
	 */
	void* opaque;
} AVChannelLayout;

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* Step size for volume control in dB */
#define SDL_VOLUME_STEP (0.75)

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define CURSOR_HIDE_DELAY 1000000

#define USE_ONEPASS_SUBTITLE_RENDER 1

static unsigned sws_flags = SWS_BICUBIC;

typedef struct MyAVPacketList {
	AVPacket* pkt;
	int serial;
} MyAVPacketList;

typedef struct PacketQueue {
	AVFifo* pkt_list;
	int nb_packets;
	int size;
	int64_t duration;
	int abort_request;
	int serial;
	SDL_mutex* mutex;
	SDL_cond* cond;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct AudioParams {
	int freq;
	AVChannelLayout ch_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} AudioParams;

typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
	AVFrame* frame;
	AVSubtitle sub;
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame */
	int64_t pos;          /* byte position of the frame in the input file */
	int width;
	int height;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;
} Frame;

typedef struct FrameQueue {
	Frame queue[FRAME_QUEUE_SIZE];
	int rindex;
	int windex;
	int size;
	int max_size;
	int keep_last;
	int rindex_shown;
	SDL_mutex* mutex;
	SDL_cond* cond;
	PacketQueue* pktq;
} FrameQueue;

enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct Decoder {
	AVPacket* pkt;
	PacketQueue* queue;
	AVCodecContext* avctx;
	int pkt_serial;
	int finished;
	int packet_pending;
	SDL_cond* empty_queue_cond;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;
	SDL_Thread* decoder_tid;
} Decoder;

typedef struct VideoState {
	SDL_Thread* read_tid;
	const AVInputFormat* iformat;
	int abort_request;
	int force_refresh;
	int paused;
	int last_paused;
	int queue_attachments_req;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	int64_t seek_rel;
	int read_pause_return;
	AVFormatContext* ic;
	int realtime;

	Clock audclk;
	Clock vidclk;
	Clock extclk;

	FrameQueue pictq;
	FrameQueue subpq;
	FrameQueue sampq;

	Decoder auddec;
	Decoder viddec;
	Decoder subdec;

	int audio_stream;

	int av_sync_type;

	double audio_clock;
	int audio_clock_serial;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	AVStream* audio_st;
	PacketQueue audioq;
	int audio_hw_buf_size;
	uint8_t* audio_buf;
	uint8_t* audio_buf1;
	unsigned int audio_buf_size; /* in bytes */
	unsigned int audio_buf1_size;
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;
	int audio_volume;
	int muted;
	struct AudioParams audio_src;
#if CONFIG_AVFILTER
	struct AudioParams audio_filter_src;
#endif
	struct AudioParams audio_tgt;
	struct SwrContext* swr_ctx;
	int frame_drops_early;
	int frame_drops_late;

	enum ShowMode {
		SHOW_MODE_NONE = -1, SHOW_MODE_VIDEO = 0, SHOW_MODE_WAVES, SHOW_MODE_RDFT, SHOW_MODE_NB
	} show_mode;
	int16_t sample_array[SAMPLE_ARRAY_SIZE];
	int sample_array_index;
	int last_i_start;
	RDFTContext* rdft;
	int rdft_bits;
	FFTSample* rdft_data;
	int xpos;
	double last_vis_time;
	SDL_Texture* vis_texture;
	SDL_Texture* sub_texture;
	SDL_Texture* vid_texture;

	int subtitle_stream;
	AVStream* subtitle_st;
	PacketQueue subtitleq;

	double frame_timer;
	double frame_last_returned_time;
	double frame_last_filter_delay;
	int video_stream;
	AVStream* video_st;
	PacketQueue videoq;
	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
	struct SwsContext* img_convert_ctx;
	struct SwsContext* sub_convert_ctx;
	int eof;

	char* filename;
	int width, height, xleft, ytop;
	int step;

#if CONFIG_AVFILTER
	int vfilter_idx;
	AVFilterContext* in_video_filter;   // the first filter in the video chain
	AVFilterContext* out_video_filter;  // the last filter in the video chain
	AVFilterContext* in_audio_filter;   // the first filter in the audio chain
	AVFilterContext* out_audio_filter;  // the last filter in the audio chain
	AVFilterGraph* agraph;              // audio filter graph
#endif

	int last_video_stream, last_audio_stream, last_subtitle_stream;

	SDL_cond* continue_read_thread;
} VideoState;