#pragma once

#include "ECS.hpp"

class EntitySystem {
public:
    bool initialize() {
        return false;
    }

    Entity* createEntity();
    void deleteEntity(Entity* entity);

private:
    std::array<Entity, k_numMaxEntities> _entities;
};