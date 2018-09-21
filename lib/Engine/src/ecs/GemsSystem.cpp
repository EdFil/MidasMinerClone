//
// Created by edgar on 20-09-2018.
//

#include "GemsSystem.hpp"

#include <SDL_render.h>

#include "Engine.hpp"
#include "EntitySystem.hpp"
#include "TransformSystem.hpp"
#include "RenderSystem.hpp"
#include "TextureManager.hpp"

static const int k_numGemsX = 8;
static const int k_numGemsY = 8;
static const int k_gemSize = 35;
static const int k_gemPadding = 8;
static const glm::vec2 k_startPosition = glm::vec2(320, 92);

bool GemsSystem::initialize(Engine* engine) {
    for(size_t y = 0, x = 0; y < k_numGemsX; y++, x = 0) {
        for(; x < k_numGemsY; x++) {
            auto transform = engine->transformSystem()->createComponent();
            transform->position = k_startPosition + glm::vec2(
                    k_gemPadding * (x + 1) + x * k_gemSize,
                    k_gemPadding * (y + 1) + y * k_gemSize);

            auto texture = engine->textureManager()->loadTexture(static_cast<TextureID>(1 + rand() % (static_cast<int>(TextureID::COUNT) - 1)));
            auto render = engine->renderSystem()->createComponent(transform, texture);

            auto entity = engine->entitySystem()->createEntity();
            entity->addComponent(transform);
            entity->addComponent(render);
        }
    }

    return false;
}
