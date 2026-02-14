// Related header
#include "exodus/ecs/systems/sprite_render.hpp"

// Local headers
#include "exodus/camera.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/rendering/renderer.hpp"

namespace {
/// The base size of a sprite texture.
constexpr int SPRITE_TEXTURE_SIZE{128};

/// The size of the screen.
constexpr exodus::Vec2f SCREEN_SIZE{1280.0F, 720.0F};

/// The near-clipping plane for depth testing.
constexpr float NEAR_PLANE{-10.0F};

/// The far-clipping plane for depth testing.
constexpr float FAR_PLANE{10.0F};

/// The scale factor to apply to sprites.
constexpr float SPRITE_SCALE{0.25F};

/// The final size of a sprite in pixels after scaling.
constexpr float SPRITE_SIZE{SPRITE_TEXTURE_SIZE * SPRITE_SCALE};
}  // namespace

namespace exodus::ecs::systems {
void sprite_render_system(Registry& registry, const Camera& camera, rendering::Renderer& renderer) {
  // Calculate the projection matrix
  const std::array projection_matrix{
      // Column 0 (scale X)
      2.0F / (SCREEN_SIZE.x / camera.get_zoom()),
      0.0F,
      0.0F,
      0.0F,

      // Column 1 (scale Y)
      0.0F,
      -2.0F / (SCREEN_SIZE.y / camera.get_zoom()),
      0.0F,
      0.0F,

      // Column 2 (scale Z)
      0.0F,
      0.0F,
      -2.0F / (FAR_PLANE - NEAR_PLANE),
      0.0F,

      // Column 3 (translation)
      -1.0F,
      1.0F,
      -(FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE),
      1.0F,
  };

  // Batch sprites by texture ID, then upload everything to the renderer
  rendering::RenderBatches batches;
  for (const auto& [sprite, transform] : registry.view<components::Sprite, components::Transform>()) {
    // Create a new render instance and add it to the batch
    rendering::RenderInstance render_instance;
    const float scaled_sprite_size{sprite.scale * SPRITE_SIZE};
    const Vec2f centre{(SCREEN_SIZE - scaled_sprite_size) / 2.0F};
    render_instance.position = (transform.position * SPRITE_SIZE) + centre;
    render_instance.scale = scaled_sprite_size;
    render_instance.depth = static_cast<float>(sprite.depth);
    batches[sprite.texture_id].emplace_back(render_instance);
  }
  const Vec2f camera_pixel_offset{camera.get_position() * -SPRITE_SIZE};
  renderer.publish(batches, projection_matrix, camera_pixel_offset);
}
}  // namespace exodus::ecs::systems
