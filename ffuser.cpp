#include "ffuser.h"

#include "ffmpeg.h"
#include "ffencode.h"
#include "ffdecode.h"

#include "easy.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QEventLoop>
using namespace std;
namespace FF{

FFUser::FFUser()
	: decoder(0)
	, encoder(0)
{
}
FFUser::~FFUser()
{
	SafeDeletePtr(decoder);
	SafeDeletePtr(encoder);
}
int FFUser::initDecode(Params params)
{
	SafeDeletePtr(decoder);
	decoder = new FFDECODE;
	connect(decoder, SIGNAL(decodeOneFrame(QByteArray, int, int)), this, SIGNAL(decodeOneFrame(QByteArray, int, int)));
	return decoder->initAsDecode(params);
}
int FFUser::releaseDecode()
{
	decoder->release();
	SafeDeletePtr(decoder);
	return 0;
}
int FFUser::initEncode(Params params)
{
	SafeDeletePtr(encoder);
	encoder = new FFENCODE;
	connect(encoder, SIGNAL(encodeOneFrame(QByteArray)), this, SIGNAL(encodeOneFrame(QByteArray)));
	return encoder->initAsEncode(params);
}
int FFUser::releaseEncode()
{
	encoder->release();
	SafeDeletePtr(encoder);
	return 0;
}
int FFUser::encode(const unsigned char *buf, int size)
{
	if (!buf || size <= 0) {
		cout << "params error"<<endl;
		return -1;
	}
	if (encoder == nullptr) {
		cout << "encoder not init"<<endl;
		return -2;
	}
	return encoder->encode(buf, size);
}
int FFUser::decode(const unsigned char *buf, int size)
{
	if (!buf || size <=0 ) {
		cout << "params error"<<endl;
		return -1;
	}
	if (decoder == nullptr) {
		cout << "decoder not init " << endl;
		return -2;
	}
	return decoder->decode(buf, size);
}

}
