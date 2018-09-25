#include <Engine.hpp>

#include "MainMenuScene.hpp"

int main(int, char const **) {
    Engine engine;
    if(engine.initialize(std::make_unique<MainMenuScene>())) {
        engine.run();
    }
    engine.cleanup();

    return 0;
}
