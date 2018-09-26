#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <unordered_set>

class Engine;
struct TextureWrapper;
typedef struct _TTF_Font TTF_Font;
typedef struct SDL_Color;
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

enum class FontID {
	Title,
	COUNT
};

class TextureManager {
public:
	TextureManager(Engine* engine);

    SDL_Texture* loadTexture(const TextureID textureID);
	std::unique_ptr<TextureWrapper> loadText(const char* text, const char* fontName, unsigned fontSize, const SDL_Color& color);
	void deleteTexture(SDL_Texture* texture);

	// Helper functions
	void preloadAllTextures();
	SDL_Texture* getRandomGemTexture();

private:
	Engine* _engine;
    std::unordered_map<TextureID, SDL_Texture*> _cachedTextures;

    std::string fullPathForTextureID(TextureID textureID) const;
};

struct TextureWrapper {
	SDL_Texture* texture;
	TextureManager* manager;

	TextureWrapper(SDL_Texture* texture, TextureManager* manager) : texture(texture), manager(manager) { }
	~TextureWrapper() {  manager->deleteTexture(texture); }
};
