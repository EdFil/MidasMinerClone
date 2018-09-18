#include <cstdint>

#include <glm/vec2.hpp>

struct SDL_Texture;
enum class TextureId;

class Sprite {
public:
    static Sprite* createSprite(TextureId textureId, const glm::vec2& position);

private:
    SDL_Texture* _texture = nullptr;
    glm::vec2 _position;
};