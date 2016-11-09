#ifndef FFDECODE_H
#define FFDECODE_H

#include "ffmpeg.h"
namespace FF {
class FFDECODE : public FFMPEG {
	Q_OBJECT
public:
	FFDECODE()
	{
		av_register_all();
	}
	~FFDECODE()
	{
	}

	int initAsDecode(Params params) override;
	int decode(const unsigned char *buf, int size) override;

private:
	int write_out_frame();
};
}
#endif // FFDECODE_H
