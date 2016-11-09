#ifndef FFENCODE_H
#define FFENCODE_H
#include "ffmpeg.h"
namespace FF {
class FFENCODE : public FFMPEG {
	Q_OBJECT
public:
	FFENCODE()
	{
		av_register_all();
	}
	~FFENCODE()
	{
	}

	int initAsEncode(Params params) override;
	int encode(const unsigned char *buf, int size) override;

private:
	int readData(const unsigned char *buf, int size, int &hasEncode);
	int read_gray_data(const unsigned char *buf, int size, int &hasEncode);
	int read_rgb_data(const unsigned char *buf, int size, int &hasEncode);
	int read_yuv_data(const unsigned char *buf, int size, int &hasEncode);
};
}

#endif // FFENCODE_H
