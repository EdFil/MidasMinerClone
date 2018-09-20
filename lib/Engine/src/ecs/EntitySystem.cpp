//
// Created by edgar on 19-09-2018.
//

#include "EntitySystem.hpp"

bool EntitySystem::initialize() {
	for(Entity& entity : _entities) {
		entity._system = this;
	}

	return true;
}

Entity *EntitySystem::createEntity() {
    for(Entity& entity : _entities) {
        if(entity.state == State::Unused) {
            entity.state = State::Used;
            return &entity;
        }
    }

    return nullptr;
}

void EntitySystem::releaseEntity(Entity* entity) {
    entity->state = State::Unused;
	entity->cleanup();
}
