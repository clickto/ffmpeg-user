#include "ffdecode.h"
#include "convert.h"
#include <QDebug>

namespace FF {
int FFDECODE::initAsDecode(Params params)
{
    m_params = params;
    av_init_packet(&packet);
    codec = avcodec_find_decoder(params.codecID);
    if (nullptr == codec) {
        qDebug() << "codec find failed"<<endl;
        return -1;
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (codecCtx == nullptr) {
        qDebug() << "codecCtx find failed" <<endl;
        return -2;
    }
    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
        codecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
    }
    codecParserCtx = av_parser_init(params.codecID);
    if (codecParserCtx == nullptr) {
        qDebug() << "parser context init failed"<< endl;
        return -3;
    }
    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        qDebug() << "codec open failed"<<endl;
        return -4;
    }
    frame = av_frame_alloc();
    if (frame == nullptr) {
        qDebug() << "frame alloc failed" << endl;
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
            qDebug() << "decode failed";
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
            qDebug()  << "decode  failed"<<endl;
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
    int count = 0;
    switch(m_params.destFmt) {
        case GRAY: {
            count = 1;
            break;
        }
        case RGB: {
            count = 3;
            break;
        }
        case YUV: {
            count = 3;
            break;
        }
        case H264:{
            return -1;
            break;
        }

    }
    for (int i = 0; i < count; i++) {
        int w = i == 0 ? frame->width : frame->width/2;
        int h = i == 0 ? frame->height : frame->height/2;
        for (int j = 0; j < h; j++) {
            data.append((const char *)pBuf[i], w);
            pBuf[i] += pStride[i];
        }
    }
    if(m_params.destFmt == RGB)
    {
        int rgbsz =frame->width*frame->height*3;
        unsigned char *rgb = new unsigned char[rgbsz];
        memset(rgb,0,rgbsz);
        convert((unsigned char*)data.data(),frame->width,frame->height,AV_PIX_FMT_YUV420P,\
                rgb,frame->width,frame->height,AV_PIX_FMT_RGB24);
        QByteArray rgbData;
        rgbData.append((const char *)rgb, rgbsz);
        emit decodeOneFrame(rgbData, frame->width, frame->height);
        delete [] rgb;
    }
    else
        emit decodeOneFrame(data, frame->width, frame->height);
    return 0;
}
}
