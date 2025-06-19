extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
}
#include <iostream>
#include <vector>


class H264deocder {
public:
	H264deocder();
	~H264deocder();

	uint8_t* decode(uint8_t* iEncodedData, int iSize);
private:
	void send(AVCodecContext* iContext, AVPacket* iPacket);
	void receive(AVCodecContext* iContext, AVFrame* iFrame);
	AVCodecContext* context; 
	AVPacket* packet;
	AVFrame* frame;
};