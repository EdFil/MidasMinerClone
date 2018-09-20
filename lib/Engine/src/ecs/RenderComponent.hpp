#pragma once

#include "ECS.hpp"

#include <glm/vec2.hpp>

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
    glm::vec<2, int> _size;

	void cleanup();

friend RenderSystem;
};