// Related header
#include "exodus/engine.hpp"

// Std headers
#include <optional>

// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/camera_follow.hpp"
#include "exodus/ecs/systems/chunk_generation.hpp"
#include "exodus/ecs/systems/input.hpp"
#include "exodus/ecs/systems/movement.hpp"
#include "exodus/ecs/systems/sprite_render.hpp"
#include "exodus/factories.hpp"
#include "exodus/generation/generator.hpp"

namespace exodus {
namespace {
/// The initial position of the player
constexpr Vec2f PLAYER_POSITION{generation::CHUNK_SIZE / 2.0F, generation::CHUNK_SIZE / 2.0F};

/// Run a system with the appropriate arguments based on its signature.
///
/// @tparam System The system to run.
/// @param registry The registry to operate on.
/// @param camera The camera to operate on.
/// @param renderer The renderer to use for drawing to the screen.
/// @param delta_time The time interval since the last time the function was called.
template <auto System>
void run_system(ecs::Registry& registry, Camera& camera, std::optional<std::reference_wrapper<rendering::Renderer>> renderer, const float delta_time) {
  if constexpr (std::is_invocable_v<decltype(System), ecs::Registry&>) {
    System(registry);
  } else if constexpr (std::is_invocable_v<decltype(System), ecs::Registry&, float>) {
    System(registry, delta_time);
  } else if constexpr (std::is_invocable_v<decltype(System), ecs::Registry&, Camera&>) {
    System(registry, camera);
  } else if constexpr (std::is_invocable_v<decltype(System), ecs::Registry&, const Camera&, rendering::Renderer&>) {
    if (renderer.has_value()) {
      System(registry, static_cast<const Camera&>(camera), renderer.value());
    }
  } else {
    static_assert(false, "System has unsupported signature");
  }
}

/// Run multiple systems with the appropriate arguments based on their signatures.
///
/// @tparam Systems The systems to run.
/// @param registry The registry to operate on.
/// @param camera The camera to operate on.
/// @param renderer The renderer to use for drawing to the screen.
/// @param delta_time The time interval since the last time the function was called.
template <auto... Systems>
void run_systems(ecs::Registry& registry, Camera& camera, const std::optional<std::reference_wrapper<rendering::Renderer>> renderer, const float delta_time) {
  (run_system<Systems>(registry, camera, renderer, delta_time), ...);
}
}  // namespace

Engine::Engine() : registry_(std::make_unique<ecs::Registry>()) {
  create_game_object(*registry_, generation::TileType::Player, PLAYER_POSITION);
}

Engine::~Engine() = default;

void Engine::update(const float delta_time) {
  run_systems<ecs::systems::chunk_generation_system, ecs::systems::camera_follow_system>(*registry_, camera_, std::nullopt, delta_time);
}

void Engine::fixed_update(const float delta_time) {
  run_systems<ecs::systems::input_system, ecs::systems::movement_system>(*registry_, camera_, std::nullopt, delta_time);
}

void Engine::render(const float delta_time, rendering::Renderer& renderer) {
  run_systems<ecs::systems::sprite_render_system>(*registry_, camera_, renderer, delta_time);
  renderer.flush();
}
}  // namespace exodus
