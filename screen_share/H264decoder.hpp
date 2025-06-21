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
	bool want_more_packet = false;
private:
	
	AVCodecContext* context; 
	AVPacket* packet;
	AVFrame* frame;
	AVBufferRef* hw_device_ctx;
	
};
static int send_packet(AVCodecContext* context, AVPacket* packet);
static int receive_frame(AVCodecContext* context, AVFrame* frame);