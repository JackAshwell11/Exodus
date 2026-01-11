// Related header
#include "exodus/engine.hpp"

// Local headers
#include "exodus/ecs/registry.hpp"

Engine::Engine() : registry_(std::make_unique<Registry>()) {}

void Engine::update(const double /*delta_time*/) const {}

void Engine::fixed_update(const double /*delta_time*/) const {}

void Engine::render() const {}
