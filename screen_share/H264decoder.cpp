#include "H264decoder.hpp"
H264deocder::H264deocder() {

	const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	this->context = avcodec_alloc_context3(codec);
	
	
	av_opt_set(this->context->priv_data, "preset", "ultrafast", 0);
	av_opt_set(this->context->priv_data, "tune", "zerolatency", 0);

	this->packet = av_packet_alloc();
	packet->data = nullptr;
	packet->size = 0;

	this->frame = av_frame_alloc();
	frame->format = context->pix_fmt;
	frame->width = context->width;
	frame->height = context->height;

	if (avcodec_open2(context,codec,nullptr)>0) {
		std::cerr << "ERROR: Cannot opening codec.\n";
		exit(1);
	}
	std::cout << "Decoder is Ready\n";

}

H264deocder::~H264deocder() {
	av_frame_free(&this->frame);
	av_packet_free(&this->packet);
	avcodec_free_context(&this->context);
}

uint8_t* H264deocder::decode(uint8_t* encodedData, int size) {

	if (!encodedData) {
		return nullptr;
	}

	packet->data = encodedData;
	packet->size = size;
	send(context, packet);
	receive(context, frame);
	return &frame->data[0][0];
}
static void send(AVCodecContext* Context, AVPacket* Packet) {
	avcodec_send_packet(Context, Packet);
}
static void receive(AVCodecContext* Context, AVFrame* Frame) {
	avcodec_receive_frame(Context, Frame);
}