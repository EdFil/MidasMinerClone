#pragma once

#include "ECS.hpp"

#include <array>

#include "TextComponent.hpp"

class Engine;
class TextComponent;
class RenderComponent;

class TextSystem {
public:
    bool initialize(Engine* engine);

	TextComponent* createComponent(RenderComponent* renderComponent);
	void releaseComponent(TextComponent* component);

	Engine* engine() const { return _engine; }

private:
	Engine* _engine;
    std::array<TextComponent, k_numMaxTextComponents> _components;
};