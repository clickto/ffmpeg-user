#include "ffmpeg.h"
#include <iostream>
using namespace std;

namespace FF {
void outError(int num)
{
	char buf[1024];
	av_strerror(num, buf, 1024);
	cout << buf << endl;
}
FFMPEG::FFMPEG()
	: codec(0)
	, codecCtx(0)
	, frame(0)
	, codecParserCtx(0)
{
}
FFMPEG::~FFMPEG()
{
	release();
}
void FFMPEG::release()
{
	if (codecCtx) {
		avcodec_close(codecCtx);
		av_free(codecCtx);
	}
	if (frame) {
		av_frame_free(&frame);
	}
	if (codecParserCtx) {
		av_parser_close(codecParserCtx);
	}
}
}
