#pragma once

#include <memory>
#include "ecs/EntitySystem.hpp"
#include "ecs/TransformSystem.hpp"
#include "ecs/RenderSystem.hpp"
#include "EventDispatcher.hpp"
#include "ecs/TextSystem.hpp"

struct SDL_Window;
struct SDL_Renderer;
class Scene;
class TextureManager;

class Engine : public ApplicationEventDelegate {
public:
	Engine();
	virtual ~Engine();

	bool initialize(std::unique_ptr<Scene>&& scene);
	void cleanup();
	void run();
	void setScene(std::unique_ptr<Scene>&& scene);

	EntitySystem* entitySystem() { return &_entitySystem; }
    TransformSystem* transformSystem() { return &_transformSystem; }
    RenderSystem* renderSystem() { return &_renderSystem; }
    TextureManager* textureManager() { return _textureManager.get(); }
    EventDispatcher* eventDispatcher() { return _eventDispatcher.get(); }
	TextSystem* textSystem() { return &_textSystem; }
	

	void onQuit() override;

	SDL_Renderer* renderer() const { return _renderer; }

private:
	void mainLoop();

private:
	EntitySystem _entitySystem;
	TransformSystem _transformSystem;
	RenderSystem _renderSystem;
	TextSystem _textSystem;
	unsigned _lastGetTicksTime = 0;

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	std::unique_ptr<Scene> _runningScene;
	std::unique_ptr<TextureManager> _textureManager;
	std::unique_ptr<EventDispatcher> _eventDispatcher;
	
	bool _isRunning = false;
};