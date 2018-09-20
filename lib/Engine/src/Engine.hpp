#pragma once

#include <memory>
#include "ecs/EntitySystem.hpp"
#include "ecs/TransformSystem.hpp"
#include "ecs/RenderSystem.hpp"
#include "EventDispatcher.hpp"

struct SDL_Window;
struct SDL_Renderer;

class TextureManager;

class Engine : public ApplicationEventDelegate {
public:
	Engine();
	~Engine();

	bool initialize();
	void cleanup();
	void run();

	void onQuit() override;

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
	std::unique_ptr<EventDispatcher> _eventDispatcher;

	bool _isRunning = false;
};