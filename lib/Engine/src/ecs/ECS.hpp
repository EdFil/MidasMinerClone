#pragma once

constexpr unsigned k_numMaxEntities = 100;
constexpr unsigned k_numMaxTransformComponents = k_numMaxEntities;
constexpr unsigned k_numMaxRenderComponents = k_numMaxEntities;

class System {
    virtual bool initialize() = 0;
    virtual bool update(float delta) = 0;
};