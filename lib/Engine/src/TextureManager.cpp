#include "TextureManager.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "Engine.hpp"
#include "Constants.hpp"

TextureManager::TextureManager(Engine* engine) : _engine(engine) {
	SDL_assert(_engine != nullptr);
}

SDL_Texture* TextureManager::loadTexture(const TextureID textureID) {
    // If texture already cached
    const auto& it = _cachedTextures.find(textureID);
    if(it != _cachedTextures.cend()) {
        return it->second;
    }

    // Try to load the image into ram
	const std::string fullPath = fullPathForTextureID(textureID);
    SDL_Surface* loadedSurface = IMG_Load(fullPath.c_str());
    if(loadedSurface == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Unable to load image %s! SDL_image Error: %s\n", fullPath.c_str(), IMG_GetError());
        return nullptr;
    }

    // Try to create and cache the texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_engine->renderer(), loadedSurface);
    if(texture != nullptr) {
        _cachedTextures[textureID] = texture;
        SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Created new texture! %s!", fullPath.c_str());
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Unable to create texture from %s! SDL Error: %s\n", fullPath.c_str(), SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);

    return texture;
}

std::string TextureManager::fullPathForTextureID(TextureID textureID) const {
    std::string basePath(RESOURCES_DIR);

    switch(textureID) {
        case TextureID::Background:
            return basePath + "Background.png";
        case TextureID::Blue:
            return basePath + "Blue.png";
        case TextureID::Green:
            return basePath + "Green.png";
        case TextureID::Purple:
            return basePath + "Purple.png";
        case TextureID::Red:
            return basePath + "Red.png";
        case TextureID::Yellow:
            return basePath + "Yellow.png";
        default:
            SDL_assert(false); // Unknown TextureID
            return "";
    }
}