#include "Engine.hpp"

#include <cstdlib>

#include <SDL.h>
#include <SDL_log.h>

#include "TextureManager.hpp"
#include "Scene.hpp"
#include <ctime>

Engine::Engine() { }
Engine::~Engine() { }

std::unique_ptr<TextureWrapper> g_text;

bool Engine::initialize(std::unique_ptr<Scene>&& scene) {
	srand(time(nullptr));

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_Init Error: %s", SDL_GetError());
		return false;
	}

	_window = SDL_CreateWindow("ConceptV2", 100, 100, 755, 600, SDL_WINDOW_SHOWN);
	if (_window == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateWindow Error: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (_renderer == nullptr) {
		SDL_DestroyWindow(_window);
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	_textureManager = std::make_unique<TextureManager>(this);
    _eventDispatcher = std::make_unique<EventDispatcher>();

	_transformSystem.initialize();
	_renderSystem.initialize();
    _eventDispatcher->initialize();

	g_text = _textureManager->loadText("Test", "OpenSans-Bold.ttf", 32, SDL_Color{255, 0, 0, 255});
	_textureManager->preloadAllTextures();
    _eventDispatcher->registerForApplicationEvents(this);
	setScene(std::move(scene));

	return true;
}

void Engine::cleanup() {
    _eventDispatcher->unregisterForApplicationEvents(this);
	g_text.reset();

	if(_renderer)
		SDL_DestroyRenderer(_renderer);
	if(_window)
		SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Engine::run() {
	_isRunning = true;
	mainLoop();
}

void Engine::setScene(std::unique_ptr<Scene>&& scene) {
	if(_runningScene != nullptr) {
		_runningScene->onDestroy();
	}

	_runningScene = std::move(scene);

	if(_runningScene != nullptr) {
		_runningScene->attachEngine(this);
		_runningScene->onCreated();
	}
}

void Engine::mainLoop() {
	while (_isRunning) {
		_eventDispatcher->update();
		//_gemsSystem->update(0.16f);

		if(_runningScene != nullptr) {
			_runningScene->update(0.16f);
		}

		// Render Scene
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		SDL_RenderClear(_renderer);

        _renderSystem.draw(_renderer);

		SDL_Rect rect{0};
		SDL_QueryTexture(g_text->texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(_renderer, g_text->texture, nullptr, &rect);

		SDL_RenderPresent(_renderer);
	}
}

void Engine::onQuit() {
    _isRunning = false;
}
