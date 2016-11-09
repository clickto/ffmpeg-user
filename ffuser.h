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
	int width;
	int height;
	int bit_rate;
	int fps;
	int gop_size;
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

	int decode(const unsigned char *srcbuf, unsigned char *dstbuf, int srcsize, int destsize);
	int encode(const unsigned char *srcbuf, unsigned char *dstbuf, int srcsize, int destsize);

private:
	FFMPEG *decoder, *encoder;
};

}
#endif // FFUSER_H
