//
// Created by edgar on 19-09-2018.
//

#include "EntityManager.hpp"

Entity *EntitySystem::createEntity() {
    for(Entity& entity : _entities) {
        if(entity.state == State::Unused) {
            entity.state = State::Used;
            return &entity;
        }
    }

    return nullptr;
}

void EntitySystem::deleteEntity(Entity* entity) {
    entity->state = State::Unused;
}
