#include <Engine.hpp>

int main(int argc, char const *argv[]) {
    Engine engine;
    if(engine.initialize()) {
        engine.run();
    }
    engine.cleanup();

    return 0;
}
