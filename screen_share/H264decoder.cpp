#include "H264decoder.hpp"

// Forward declare static functions
static int send_packet(AVCodecContext* context, AVPacket* packet);
static int receive_frame(AVCodecContext* context, AVFrame* frame);

H264deocder::H264deocder() {
    const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "ERROR: H264 decoder not found.\n";
        exit(1);
    }

    this->context = avcodec_alloc_context3(codec);
    if (!this->context) {
        std::cerr << "ERROR: Cannot allocate codec context.\n";
        exit(1);
    }

    // Remove encoder-specific options - decoders don't need preset/tune

    this->packet = av_packet_alloc();
    if (!this->packet) {
        std::cerr << "ERROR: Cannot allocate packet.\n";
        exit(1);
    }
    packet->data = nullptr;
    packet->size = 0;

    this->frame = av_frame_alloc();
    if (!this->frame) {
        std::cerr << "ERROR: Cannot allocate frame.\n";
        exit(1);
    }

    // Don't set frame properties here - they'll be set by the decoder

    // Fix: avcodec_open2 returns 0 on success, negative on error
    if (avcodec_open2(context, codec, nullptr) < 0) {
        std::cerr << "ERROR: Cannot open codec.\n";
        exit(1);
    }

    std::cout << "Decoder is Ready\n";
}

H264deocder::~H264deocder() {
    av_frame_free(&this->frame);
    av_packet_free(&this->packet);
    avcodec_free_context(&this->context);
}

AVFrame* H264deocder::decode(uint8_t* encodedData, int size) {
    if (!encodedData || size <= 0) {
        std::cout << "null1\n";
        return nullptr;
    }

    packet->data = encodedData;
    packet->size = size;

    // Send packet to decoder
    int ret = send_packet(context, packet);
    if (ret < 0) {
        std::cerr << "ERROR: Failed to send packet to decoder: " << ret << "\n";
        return nullptr;
    }

    // Receive decoded frame
    ret = receive_frame(context, frame);
    if (ret < 0) {
        if (ret == AVERROR(EAGAIN)) {
            // Need more input data
            std::cout << "null2\n";

            return nullptr;
        }
        else if (ret == AVERROR_EOF) {
            // End of stream
            std::cout << "null3\n";

            return nullptr;
        }
        else {
            std::cerr << "ERROR: Failed to receive frame from decoder: " << ret << "\n";
            return nullptr;
        }
    }

    // Return pointer to the Y plane of the decoded frame
    return frame;
}

static int send_packet(AVCodecContext* context, AVPacket* packet) {
    return avcodec_send_packet(context, packet);
}

static int receive_frame(AVCodecContext* context, AVFrame* frame) {
    return avcodec_receive_frame(context, frame);
}