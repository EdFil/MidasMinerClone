#include "TextSystem.hpp"

#include "Engine.hpp"
#include "TextComponent.hpp"
#include "RenderComponent.hpp"

bool TextSystem::initialize(Engine* engine) {
	_engine = engine;
	for(TextComponent& component : _components) {
		component._system = this;
		component.type = ComponentType::Text;
	}

	return true;
}

TextComponent* TextSystem::createComponent(RenderComponent* renderComponent) {
    for(TextComponent& component : _components) {
        if(!component.isUsed) {
            component.initialize(renderComponent);
			component.isUsed = true;
            return &component;
        }
    }

    return nullptr;
}

void TextSystem::releaseComponent(TextComponent* component) {
	component->isUsed = false;
}
