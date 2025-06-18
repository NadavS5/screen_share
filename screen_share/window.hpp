#include <SDL.h>

class Window {
public:
	Window();
	~Window();
	void Update();
	void DrawImage(int* buffer);
private:
	SDL_Window* window;
	SDL_Surface* window_surface;
	SDL_Renderer* renderer;
	SDL_Texture* buffer;
};