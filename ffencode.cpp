#include "ffencode.h"
#include <iostream>
using namespace std;

namespace FF{
int FFENCODE::initAsEncode(Params params)
{
	m_params = params;
	int ret = 0;
	codec = avcodec_find_encoder(params.codecID);
	if (codec == nullptr) {
		cout << "find encode failed" << endl;
		return -1;
	}
	codecCtx = avcodec_alloc_context3(codec);
	if (codecCtx == nullptr) {
		cout << "alloc codec context failed"<< endl;
		return -2;
	}
	codecCtx->width     = params.width;
	codecCtx->height    = params.height;
	codecCtx->bit_rate  = params.bit_rate;
	AVRational r = {1, params.fps};
	codecCtx->time_base = r;
	codecCtx->gop_size  = params.gop_size;
	codecCtx->max_b_frames = params.max_b_frames;
	codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	ret = avcodec_open2(codecCtx, codec, NULL);
	if ( ret< 0) {
		cout << "open codec failed"<<endl;
		outError(ret);
		return -3;
	}
	frame = av_frame_alloc();
	if (frame == nullptr) {
		cout << "alloc frame failed" << endl;
		return -4;
	}
	frame->width = codecCtx->width;
	frame->height = codecCtx->height;
	frame->format = codecCtx->pix_fmt;
	ret = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 32) ;
	if (ret < 0) {
		cout <<"alloc image failed"<<endl;
		outError(ret);
		return -5;
	}
	return 0;
}
int FFENCODE::encode(const unsigned char *buf, int size)
{
	int ret = 0;
	int hasEncodeSize = 0;
	int color_size = m_params.width * m_params.height;
	int count = 0;
	int got_packet = 0;
	QByteArray data;
	while (hasEncodeSize < size) {
		ret = readData(buf, color_size, hasEncodeSize);
		if (ret <0) {
			cout <<"read data failed" << endl;
			break;
		}
		av_init_packet(&packet);
		packet.data = NULL;
		packet.size = 0;
		count++;
		frame->pts = count;
		got_packet = 0;
		ret = avcodec_encode_video2(codecCtx, &packet, frame, &got_packet);
		if (ret < 0) {
			cout << "encode failed "<< endl;
			outError(ret);
			return -1;
		}
		if (got_packet) {
			data.clear();
			data.append((const char *)packet.data, packet.size);
			emit encodeOneFrame(data);
			av_packet_unref(&packet);
		}
	}
	do {
		av_init_packet(&packet);
		packet.data = NULL;
		packet.size = 0;
		ret = avcodec_encode_video2(codecCtx, &packet, NULL, &got_packet);
		if (ret < 0) {
			cout << "encode failed "<< endl;
			outError(ret);
			return -1;
		}
		if (got_packet) {
			data.clear();
			data.append((const char *)packet.data, packet.size);
			emit encodeOneFrame(data);
			av_packet_unref(&packet);
		}
	} while(got_packet);
	return 0;
}
int FFENCODE::readData(const unsigned char *buf, int size, int &hasEncode)
{
	int ret = 0;
	switch (m_params.srcFmt) {
		case GRAY : {
			ret = read_gray_data(buf, size, hasEncode);
			break;
		}
		case RGB : {
			ret = read_rgb_data(buf, size, hasEncode);
			break;
		}
		case YUV : {
			ret = read_yuv_data(buf, size, hasEncode);
			break;
		}
		case H264 : {
			ret = -1;
			break;
		}
		default : {
			ret = -2;
			break;
		}
	}
	return ret;
}
int FFENCODE::read_gray_data(const unsigned char *buf, int size, int &hasEncode)
{
	memcpy((unsigned char *)frame->data[0], buf + hasEncode, size);
	memset((unsigned char *)frame->data[1], 0x80, size / 4);
	memset((unsigned char *)frame->data[2], 0x80, size / 4);
	hasEncode += size + size / 2;
	return 0;
}
int FFENCODE::read_rgb_data(const unsigned char *buf, int size, int &hasEncode)
{
	memcpy((unsigned char *)frame->data[0], buf + hasEncode, size);
	memcpy((unsigned char *)frame->data[1], buf + hasEncode + size, size);
	memcpy((unsigned char *)frame->data[2], buf + hasEncode + size + size, size);
	hasEncode += size * 3;
	return 0;
}
int FFENCODE::read_yuv_data(const unsigned char *buf, int size, int &hasEncode)
{
	Q_UNUSED(size)
	//color == 0 : Y value
	//color == 1 : U value
	//color == 2 : V value
	int color_stride, color_width, color_height, color_size;
	for (int color = 0; color < 3; color++) {
		color_stride = frame->linesize[color];
		color_width	 = color == 0 ? m_params.width : m_params.width / 2;
		color_height = color == 0 ? m_params.height : m_params.height / 2;
		color_size	 = color_width * color_height;
		if (color_width == color_stride){
			memcpy((unsigned char *)frame->data[color], buf + hasEncode, color_size);
			hasEncode += color_size;
		} else {
			for (int i = 0; i < color_height; i++) {
				memcpy((char *)frame->data[color] + color_stride * i, buf + hasEncode, color_width);
				hasEncode += color_width;
			}
		}
	}
	return 0;
}
}
