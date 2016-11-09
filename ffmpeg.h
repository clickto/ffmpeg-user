#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>
#include <QByteArray>
#include "ffuser.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
}

namespace FF{
void outError(int num);
class FFMPEG : public QObject
{
	Q_OBJECT
public:
	FFMPEG();
	virtual ~FFMPEG();

	virtual int initAsDecode(Params params){return 0;}
	virtual int decode(const unsigned char *buf, int size) {return 0;}

	virtual int initAsEncode(Params params) {return 0;}
	virtual int encode(const unsigned char *buf, int size) {return 0;}

	virtual void release();
public:
	AVCodec *codec;
	AVCodecContext *codecCtx;
	AVFrame *frame;
	AVPacket packet;
	AVCodecParserContext *codecParserCtx;
	Params m_params;
signals:
	void encodeOneFrame(QByteArray data);
	void decodeOneFrame(QByteArray data);
public slots:
};
}
#endif // FFMPEG_H
