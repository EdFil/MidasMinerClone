#pragma once

#include "ECS.hpp"

class TransformComponent;
struct SDL_Renderer;
struct SDL_Texture;

class RenderComponent : public Component {
public:
    static constexpr ComponentType TYPE = ComponentType::Render;

    bool initialize(TransformComponent* transformComponent, SDL_Texture* texture);

    void draw(SDL_Renderer* renderer);

private:
    SDL_Texture* _texture;
    TransformComponent* _transformComponent;
};