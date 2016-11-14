﻿#include "ffdecode.h"
#include <iostream>
using namespace std;

namespace FF {
int FFDECODE::initAsDecode(Params params)
{
	m_params = params;
	av_init_packet(&packet);
	codec = avcodec_find_decoder(params.codecID);
	if (nullptr == codec) {
		cout << "codec find failed"<<endl;
		return -1;
	}
	codecCtx = avcodec_alloc_context3(codec);
	if (codecCtx == nullptr) {
		cout << "codecCtx find failed" <<endl;
		return -2;
	}
	if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
		codecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
	}
	codecParserCtx = av_parser_init(params.codecID);
	if (codecParserCtx == nullptr) {
		cout << "parser context init failed"<< endl;
		return -3;
	}
	if (avcodec_open2(codecCtx, codec, NULL) < 0) {
		cout << "codec open failed"<<endl;
		return -4;
	}
	frame = av_frame_alloc();
	if (frame == nullptr) {
		cout << "frame alloc failed" << endl;
		return -5;
	}
	return 0;
}
int FFDECODE::decode(const unsigned char *buf, int size)
{
	int ret = 0;
	int needDecodeSize = size;
	int got_picture = 0;
	int len = 0;
	const unsigned char * pDataPtr = nullptr;
	pDataPtr = buf;
	while (needDecodeSize > 0) {
		len = av_parser_parse2(codecParserCtx, codecCtx,
							   &packet.data, &packet.size,
							   pDataPtr, needDecodeSize,
							   AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		pDataPtr += len;
		needDecodeSize -=len;
		if (packet.size == 0) {
			continue;
		}
		ret = avcodec_decode_video2(codecCtx, frame, &got_picture, &packet);
		if (ret < 0) {
			cout << "decode failed";
			outError(ret);
			return ret;
		}
		if (got_picture) {
			write_out_frame();
		}
	}
	packet.data = NULL;
	packet.size = 0;
	do {
		ret = avcodec_decode_video2(codecCtx, frame, &got_picture, &packet);
		if (ret < 0) {
			cout  << "decode  failed"<<endl;
			return ret;
		}
		if (got_picture) {
			write_out_frame();
		}
	} while(got_picture);
	return 0;
}
int FFDECODE::write_out_frame()
{
	QByteArray data;
	uint8_t **pBuf = frame->data;
	int *pStride = frame->linesize;
	for (int i = 0; i < 3; i++) {
		int w = i == 0 ? frame->width : frame->width/2;
		int h = i == 0 ? frame->height : frame->height/2;
		for (int j = 0; j < h; j++) {
			data.append((const char *)pBuf[i], w);
			pBuf[i] += pStride[i];
		}
	}
	emit decodeOneFrame(data, frame->width, frame->height);
	return 0;
}
}
