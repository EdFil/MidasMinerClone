#pragma once

#include "Component.hpp"
#include <string>

typedef struct SDL_Color;
class TextSystem;
class RenderComponent;

class TextComponent : public Component {
public:
	void initialize(RenderComponent* renderComponent);
	void release() override;

	const std::string& text() const { return _string; }
	void setText(const std::string& text, const std::string& fontName, const unsigned fontSize, const SDL_Color& color);

private:
	TextSystem* _system{ nullptr };
	RenderComponent* _renderComponent{ nullptr };
	std::string _string;

	friend TextSystem;
};