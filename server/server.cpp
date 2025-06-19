#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}


int main()
{
    std::cout << "Hello World!\n";

    const AVCodec* codec = avcodec_find_encoder_by_name("libx264rgb");
    auto context = avcodec_alloc_context3(codec);


}