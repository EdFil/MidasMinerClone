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
        if(!entity._isUsed) {
            entity._isUsed = true;
            return &entity;
        }
    }

    return nullptr;
}

void EntitySystem::releaseEntity(Entity* entity) {
    entity->_isUsed = false;
	entity->cleanup();
}
