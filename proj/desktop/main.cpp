#include <Engine.hpp>

#include "MainMenuScene.hpp"

int main(int, char const **) {
    Engine* engine = new Engine();
    if(engine->initialize(std::make_unique<MainMenuScene>())) {
        engine->run();
    }

    engine->cleanup();
    delete engine;

    return 0;
}
