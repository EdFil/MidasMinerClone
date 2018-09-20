#pragma once

#include "ECS.hpp"

#include <array>
#include <SDL_system.h>

#include "RenderComponent.hpp"

struct SDL_texture;

class RenderSystem {
public:
    bool initialize();
	void releaseComponent(RenderComponent* component);
    bool draw(SDL_Renderer* renderer);

    RenderComponent* createComponent(TransformComponent* transformComponent, SDL_Texture* texture);

private:
    std::array<RenderComponent, k_numMaxRenderComponents> _components;
};