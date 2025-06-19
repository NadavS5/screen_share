extern "C" {
	#include <libavcodec/avcodec.h>

}
#include <SDL.h>
#define COLOR_FORMAT SDL_PIXELFORMAT_RGBA8888

class Window {
public:
	Window();
	~Window();
	void Update();
	void DrawFrame(AVFrame* frame);
	void Fill(SDL_Color* color);
private:
	SDL_Window* window;
	SDL_Surface* window_surface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};