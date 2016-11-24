#ifndef FFUSER_H
#define FFUSER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <QObject>

namespace FF {

class FFMPEG;

enum Format{
	GRAY = 0,
	RGB,
	YUV,
	H264,
	FORMAT_NUMBER
};
typedef struct _Params{
    /**
     * picture width / height.
     *
     * @note Those fields may not match the values of the last
     * AVFrame output by avcodec_decode_video2 due frame
     * reordering.
     *
     * - encoding: MUST be set by user.
     * - decoding: May be set by the user before opening the decoder if known e.g.
     *             from the container. Some decoders will require the dimensions
     *             to be set by the caller. During decoding, the decoder may
     *             overwrite those values as required while parsing the data.
     */
	int width;
	int height;

    /**
     * the average bitrate
     * - encoding: Set by user; unused for constant quantizer encoding.
     * - decoding: Set by user, may be overwritten by libavcodec
     *             if this info is available in the stream
     */
	int bit_rate;

	//fps is used to calc time_base.
	//	AVRational r = {1, params.fps};
	//	codecCtx->time_base = r;
	/**
	 * This is the fundamental unit of time (in seconds) in terms
	 * of which frame timestamps are represented. For fixed-fps content,
	 * timebase should be 1/framerate and timestamp increments should be
	 * identically 1.
	 * This often, but not always is the inverse of the frame rate or field rate
	 * for video. 1/time_base is not the average frame rate if the frame rate is not
	 * constant.
	 *
	 * Like containers, elementary streams also can store timestamps, 1/time_base
	 * is the unit in which these timestamps are specified.
	 * As example of such codec time base see ISO/IEC 14496-2:2001(E)
	 * vop_time_increment_resolution and fixed_vop_rate
	 * (fixed_vop_rate == 0 implies that it is different from the framerate)
	 *
	 * - encoding: MUST be set by user.
	 * - decoding: the use of this field for decoding is deprecated.
	 *             Use framerate instead.
	 */
	int fps;

    /**
     * the number of pictures in a group of pictures, or 0 for intra_only
     * - encoding: Set by user.
     * - decoding: unused
     */
	int gop_size;

    /**
     * maximum number of B-frames between non-B-frames
     * Note: The output will be delayed by max_b_frames+1 relative to the input.
     * - encoding: Set by user.
     * - decoding: unused
     */
	int max_b_frames;

	int srcFmt;
	int destFmt;
	AVCodecID codecID;

    _Params()
		: width(256)
		, height(256)
		, bit_rate(4 * 1024 * 1024)
		, fps(25)
		, gop_size(12)
		, max_b_frames(1)
		, srcFmt(RGB)
		, destFmt(H264)
		, codecID(AV_CODEC_ID_H264)
	{
	}
}Params;


class FFUser : public QObject
{
	Q_OBJECT
public:
	FFUser();
	~FFUser();

	int initDecode(Params params);
	int releaseDecode();

	int initEncode(Params params);
	int releaseEncode();

	int decode(const unsigned char *buf,  int size);
	int encode(const unsigned char *buf, int size);
signals:
	void encodeOneFrame(QByteArray data);
	void decodeOneFrame(QByteArray data, int width, int height);
private:
	FFMPEG *decoder, *encoder;
};

}
#endif // FFUSER_H
