#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>
#include "window.hpp"
#include <thread>

#include "Communication.hpp"
#include "H264decoder.hpp"

int main(int argc, char* argv) {
	SDL_SetMainReady();

    
    Window win;
    win.Update();
    SDL_Color c = { 0,0,255,255 };
    win.Fill(&c);
    win.Update();
    SDL_Event e;
    Stream s("192.168.0.231", 8999);

    H264deocder decoder;
    bool running = true;
    while(running){
        SDL_PollEvent(&e);

        switch (e.type) {
        case SDL_QUIT:
            break;
            
        case SDL_WINDOWEVENT:
            switch (e.window.event) {

            case SDL_WINDOWEVENT_CLOSE:   // exit game
                running = false;
                break;

            default:
                break;
            }
            break;
        }
        if(running){
            std::vector<char> encodedFrame = s.RecvBySize();
            //std::cout << "1\n";
            AVFrame* frame = decoder.decode((uint8_t*)encodedFrame.data(), encodedFrame.capacity());
            win.DrawFrame(frame);
            
            win.Update();

        }
        
        
    }
    std::cout << "Exiting..." << std::endl;
    return 0;
}