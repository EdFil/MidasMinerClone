#include "TextureManager.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Engine.hpp"
#include "Constants.hpp"

const char TextureManager::k_defaultFontName[] = "OpenSans-Bold.ttf";

TextureManager::TextureManager(Engine* engine) : _engine(engine) {
	SDL_assert(_engine != nullptr);

	IMG_Init(IMG_InitFlags::IMG_INIT_PNG);
	TTF_Init();
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

SDL_Texture* TextureManager::loadText(const char* text, const char* fontName, unsigned fontSize, const SDL_Color& color) {
	std::string fullPath = std::string(RESOURCES_DIR) + fontName;

	TTF_Font* font = nullptr;
	// TODO: Cache fonts
	//const auto it = _cachedFonts.find({ std::string(fontName), fontSize });
	//if (it != _cachedFonts.cend()) {
	//	font = it->second;
	//}

	if (font == nullptr) {
		font = TTF_OpenFont(fullPath.c_str(), fontSize);
		if (font == nullptr) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Could not open font. SDL Error: %s", TTF_GetError());
			return nullptr;
		}
	}
	
	SDL_Surface* surfaceText = TTF_RenderText_Solid(font, text, color);
	if(surfaceText == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Could not generate surface for text. SDL Error: %s", TTF_GetError());
		return nullptr;
	}


	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_engine->renderer(), surfaceText);
	SDL_FreeSurface(surfaceText);
	TTF_CloseFont(font);

	return textTexture;
}

void TextureManager::deleteTexture(SDL_Texture* texture) {
	SDL_DestroyTexture(texture);

	for (auto it = _cachedTextures.begin(); it != _cachedTextures.end(); ++it) {
		if(it->second == texture) {
			_cachedTextures.erase(it);
			break;
		}

	}
}

void TextureManager::preloadAllTextures() {
	for(int i = static_cast<int>(TextureID::Background); i < static_cast<int>(TextureID::COUNT); i++) {
		loadTexture(static_cast<TextureID>(i));
	}
}

SDL_Texture* TextureManager::getRandomGemTexture() {
	return loadTexture(static_cast<TextureID>(1 + rand() % (static_cast<int>(TextureID::COUNT) - 1)));
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
		case TextureID::Cross:
			return basePath + "Cross.png";
        default:
            SDL_assert(false); // Unknown TextureID
            return "";
    }
}
