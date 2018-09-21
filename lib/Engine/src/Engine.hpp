#pragma once

#include <memory>
#include "ecs/EntitySystem.hpp"
#include "ecs/TransformSystem.hpp"
#include "ecs/RenderSystem.hpp"
#include "EventDispatcher.hpp"

struct SDL_Window;
struct SDL_Renderer;

class TextureManager;
class GemsSystem;

class Engine : public ApplicationEventDelegate {
public:
	Engine();
	~Engine();

	bool initialize();
	void cleanup();
	void run();

	EntitySystem* entitySystem() { return &_entitySystem; }
    TransformSystem* transformSystem() { return &_transformSystem; }
    RenderSystem* renderSystem() { return &_renderSystem; }
    TextureManager* textureManager() { return _textureManager.get(); }

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
	std::unique_ptr<GemsSystem> _gemsSystem;

	bool _isRunning = false;
};