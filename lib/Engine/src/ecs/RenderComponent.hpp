#pragma once

#include <glm/ext/vector_int2.hpp>
#include <ecs/Component.hpp>

class TransformComponent;
class RenderSystem;
struct SDL_Renderer;
struct SDL_Texture;

class RenderComponent : public Component {
public:
	void release() override;

    bool initialize(TransformComponent* transformComponent, SDL_Texture* texture);
	void setTexture(SDL_Texture* texture);
	void deleteTexture();

    void draw(SDL_Renderer* renderer);

    TransformComponent* transformComponent() const { return _transformComponent; }
    const glm::vec<2, int>& size() const { return _size; }

    bool isVisible() const { return _isVisible; }
    void setIsVisible(bool value) { _isVisible = value; }

private:
    SDL_Texture* _texture;
	RenderSystem* _system;
    TransformComponent* _transformComponent;
    glm::ivec2 _size;
    bool _isVisible = true;

	void cleanup();

friend RenderSystem;
};