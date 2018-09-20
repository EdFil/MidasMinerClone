#pragma once

#include "ECS.hpp"

class TransformComponent;
class RenderSystem;
struct SDL_Renderer;
struct SDL_Texture;

class RenderComponent : public Component {
public:
	void release() override;

    bool initialize(TransformComponent* transformComponent, SDL_Texture* texture);

    void draw(SDL_Renderer* renderer);

private:
    SDL_Texture* _texture;
	RenderSystem* _system;
    TransformComponent* _transformComponent;

	void cleanup();

friend RenderSystem;
};