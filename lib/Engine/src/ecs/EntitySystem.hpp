#pragma once

#include <array>

#include "ECS.hpp"
#include "Entity.hpp"

class EntitySystem {
public:
	bool initialize();

    Entity* createEntity();
    void releaseEntity(Entity* entity);

private:
    std::array<Entity, k_numMaxEntities> _entities;
};