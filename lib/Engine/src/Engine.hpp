#pragma once

#include <memory>
#include "ecs/EntityManager.hpp"
#include "ecs/TransformSystem.hpp"
#include "ecs/RenderSystem.hpp"

struct SDL_Window;
struct SDL_Renderer;

class TextureManager;

class Engine {
public:
	Engine();
	~Engine();

	bool initialize();
	void cleanup();
	void run();

	SDL_Renderer* renderer() const { return _renderer; }

private:
	void mainLoop();

private:
	EntitySystem _entitySystem;
	TransformSystem _transformSystem;
	RenderSystem _renderSystem;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	std::unique_ptr<TextureManager> _textureManager;

	bool _isRunning = false;
};