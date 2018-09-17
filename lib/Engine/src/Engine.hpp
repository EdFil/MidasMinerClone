#pragma once

struct SDL_Window;
struct SDL_Renderer;

class Engine {
public:
	bool initialize();
	void cleanup();

	void run();

private:
	void mainLoop();

private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	bool _isRunning = false;
};