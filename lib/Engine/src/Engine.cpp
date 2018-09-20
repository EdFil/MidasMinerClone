#include "Engine.hpp"

#include <cstdlib>

#include <SDL.h>
#include <SDL_log.h>

#include "TextureManager.hpp"
#include "ecs/ECS.hpp"

SDL_Texture* _background;
Entity* _entity;

Engine::Engine() { }
Engine::~Engine() { }

bool Engine::initialize() {
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

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
	_background = _textureManager->loadTexture(TextureID::Background);

	_transformSystem.initialize();
	_renderSystem.initialize();

	TransformComponent* transformComponent = _transformSystem.createComponent();
	RenderComponent* renderComponent = _renderSystem.createComponent(transformComponent, _textureManager->loadTexture(TextureID:: Red));

	_entity = _entitySystem.createEntity();
	_entity->addComponent(transformComponent);
	_entity->addComponent(renderComponent);

	return true;
}

void Engine::cleanup() {
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

void Engine::mainLoop() {
    Uint8 *selectedColor = nullptr;
    bool *selectedBool = nullptr;
    Uint8 r = static_cast<Uint8>(rand() % 256), g = static_cast<Uint8>(rand() % 256), b = static_cast<Uint8>(rand() % 256);
    bool rbool = false, gbool = false, bbool = false;

	while (_isRunning) {
		// Fetch SDL_Events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				_isRunning = false;
			}
		}

        // <DEBUG>
        switch(rand() % 3) {
            case 0: selectedColor = &r; selectedBool = &rbool; break;
            case 1: selectedColor = &g; selectedBool = &gbool; break;
            case 2: selectedColor = &b; selectedBool = &bbool; break;
        }

        if(*selectedBool) {
            *selectedColor = static_cast<Uint8>(*selectedColor + 1);
            if(*selectedColor > 254)
                *selectedBool = !(*selectedBool);
        } else {
            *selectedColor = static_cast<Uint8>(*selectedColor - 1);
            if(*selectedColor < 1)
                *selectedBool = !(*selectedBool);
        }
        // </DEBUG>

		TransformComponent* component = static_cast<TransformComponent*>(_entity->getComponentWithType(ComponentType::Transform));
		if(component->position.x > 10) {
			_entity->release();
		}

		// Render Scene
		SDL_SetRenderDrawColor(_renderer, r, g, b, 255);
		SDL_RenderClear(_renderer);
	
		SDL_RenderCopy(_renderer, _background, nullptr, nullptr);

        _renderSystem.draw(_renderer);

		SDL_RenderPresent(_renderer);
	}
}
