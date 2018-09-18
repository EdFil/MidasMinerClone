#include <Engine.hpp>

int main(int, char const **) {
    Engine engine;
    if(engine.initialize()) {
        engine.run();
    }
    engine.cleanup();

    return 0;
}
