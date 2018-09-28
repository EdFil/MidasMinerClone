#include "TextComponent.hpp"

#include <SDL_assert.h>

#include "TextSystem.hpp"
#include "TextureManager.hpp"
#include "Engine.hpp"
#include "RenderComponent.hpp"

void TextComponent::initialize(RenderComponent* renderComponent) {
	SDL_assert(renderComponent != nullptr);
	_renderComponent = renderComponent;
}

void TextComponent::release() {
	_renderComponent->deleteTexture();
	_string.clear();
	_system->releaseComponent(this);
}

void TextComponent::setText(const std::string& text, const std::string& fontName, const unsigned fontSize, const SDL_Color& color) {
	if(_string != text) {
		_renderComponent->deleteTexture();

		if(!text.empty()) {
			_string = text;
			SDL_Texture* texture = _system->engine()->textureManager()->loadText(text.c_str(), fontName.c_str(), fontSize, color);
			_renderComponent->setTexture(texture);
		}
	}
}
