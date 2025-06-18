#include <SDL.h>
#include <iostream>

#include "window.hpp"

Window::Window() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize the SDL2 library\n";
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Screen Share Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, 0);
    

    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return;
    }
    this->window = window;

    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 1920, 1080);
}
Window::~Window() {}
void Window::Update() {
    SDL_UpdateWindowSurface(this->window);
}