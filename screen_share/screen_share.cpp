#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>
#include "window.hpp"
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

int main(int argc, char* argv) {
	SDL_SetMainReady();

    
    Window win;
    win.Update();

    SDL_Event e;
    while (SDL_PollEvent(&e) > 0){
        switch (e.type){
        case SDL_QUIT:
            break;
        }
    }

}