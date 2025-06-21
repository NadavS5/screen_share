#include <SDL.h>
#include <iostream>

#include "window.hpp"

Window::Window()
    : window(nullptr), renderer(nullptr), texture(nullptr), window_surface(nullptr) // <- fix here
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize the SDL2 library\n";
        return;
    }

    SDL_Window* win = SDL_CreateWindow("Screen Share Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);

    if (!win) {
        std::cerr << "Failed to create window" << std::endl;
        return;
    }

    this->window = win;
    /*this->window_surface = SDL_GetWindowSurface(win);
    if (window_surface == NULL) {
        std::cerr << "SDL_GetWindowSurface failed: " << SDL_GetError() << "\n";
        return;
    }*/
    this->renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return;
    }
    // Example using the window surface
    this->texture= SDL_CreateTexture(renderer, SDL_PIXELFORMAT_NV12, SDL_TEXTUREACCESS_TARGET, 1920, 1080);
    //this->texture = SDL_CreateTextureFromSurface(renderer, window_surface);
    if (texture == NULL) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        return;
    }
}
Window::~Window() {}
void Window::Update() {
    //SDL_UpdateWindowSurface(this->window);
}
void Window::Fill(SDL_Color* color) {
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    SDL_RenderClear(renderer);  // fills the entire screen with the draw color
    SDL_RenderPresent(renderer);

}


void Window::DrawFrame(AVFrame* frame) {
    if (frame == nullptr) {
        std::cerr << "Error: Decoded frame is null.\n";
        exit(-1);
        return;
    }
    
    int w, h;
    SDL_GetWindowSize(this->window, &w, &h);

    SDL_Rect r = { 0, 0, w, h };

    std::cout << (this->texture == nullptr) << std::endl;
    
    int ret = SDL_UpdateNVTexture(
        texture, &r,
        frame->data[0], frame->linesize[0], // Y plane
        frame->data[1], frame->linesize[1] // U plane
    );
    if (ret < 0) {
        std::cerr << "SDL_UpdateNVTexture failed: " << SDL_GetError() << "\n";
        
        return;
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
