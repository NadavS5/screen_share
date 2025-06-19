extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
}
#include <iostream>
#include <vector>


class H264deocder {
public:
	H264deocder();
	~H264deocder();
	AVFrame* decode(uint8_t* iEncodedData, int iSize);
private:
	
	AVCodecContext* context; 
	AVPacket* packet;
	AVFrame* frame;
};
static void send(AVCodecContext* iContext, AVPacket* iPacket);
static void receive(AVCodecContext* iContext, AVFrame* iFrame);