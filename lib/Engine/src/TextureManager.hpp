#pragma once

#include <string>
#include <unordered_map>

class Engine;
struct SDL_Texture;

enum class TextureID {
    Background = 0,
    Blue = 1,
    Green,
    Purple,
    Red,
    Yellow,
    COUNT
};

class TextureManager {
public:
	TextureManager(Engine* engine);

    SDL_Texture* loadTexture(const TextureID textureID);

	// Helper functions
	void preloadAllTextures();
	SDL_Texture* getRandomGemTexture();

private:
	Engine* _engine;
    std::unordered_map<TextureID, SDL_Texture*> _cachedTextures;

    std::string fullPathForTextureID(TextureID textureID) const;
};