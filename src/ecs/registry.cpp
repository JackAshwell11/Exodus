// Related header
#include "exodus/ecs/registry.hpp"

// Std headers
#ifdef __GNUC__
#include <cxxabi.h>
#endif

auto type_name_from_info(const std::type_info& info) -> std::string {
#ifdef __GNUC__
  int status{0};
  const std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(info.name(), nullptr, nullptr, &status),
                                                   std::free};
  return (status == 0 && res) ? std::string(res.get()) : info.name();
#else
  return std::string(info.name()).substr(7);
#endif
}

auto Registry::create_game_object(const exodus::GameObjectType game_object_type) -> exodus::GameObjectID {
  // Get the game object ID to use
  exodus::GameObjectID game_object_id{0};
  if (!recycled_ids_.empty()) {
    // Reuse a recycled ID if available
    game_object_id = recycled_ids_.front();
    recycled_ids_.pop();
  } else {
    // Use the next game object ID
    game_object_id = next_game_object_id_;
    next_game_object_id_++;
  }

  // Create the game object
  game_object_types_[game_object_id] = game_object_type;
  game_object_ids_[game_object_type].push_back(game_object_id);
  return game_object_id;
}

void Registry::delete_game_object(const exodus::GameObjectID game_object_id) {
  // Check if the game object is registered or not
  if (!has_game_object(game_object_id)) {
    throw RegistryError(game_object_id);
  }

  // Notify the callbacks then delete the game object
  for (auto& component_map : components_ | std::views::values) {
    component_map.erase(game_object_id);
  }
  std::erase(game_object_ids_[get_game_object_type(game_object_id)], game_object_id);
  game_object_types_.erase(game_object_id);
  recycled_ids_.push(game_object_id);
}

void Registry::clear_game_objects(const std::unordered_set<exodus::GameObjectID>& game_object_ids_to_preserve) {
  std::unordered_set<exodus::GameObjectID> ids_to_delete;
  for (const auto& game_object_id : game_object_types_ | std::views::keys) {
    if (!game_object_ids_to_preserve.contains(game_object_id)) {
      ids_to_delete.insert(game_object_id);
    }
  }
  for (const auto game_object_id : ids_to_delete) {
    delete_game_object(game_object_id);
  }
}

auto Registry::has_game_object(const exodus::GameObjectID game_object_id) const -> bool {
  return game_object_types_.contains(game_object_id);
}

auto Registry::get_game_object_type(const exodus::GameObjectID game_object_id) const -> exodus::GameObjectType {
  if (!game_object_types_.contains(game_object_id)) {
    throw RegistryError(game_object_id);
  }
  return game_object_types_.at(game_object_id);
}

auto Registry::get_game_object_ids(const exodus::GameObjectType game_object_type) const
    -> std::vector<exodus::GameObjectID> {
  const auto ids{game_object_ids_.find(game_object_type)};
  return ids != game_object_ids_.end() ? ids->second : std::vector<exodus::GameObjectID>{};
}

void Registry::mark_for_deletion(const exodus::GameObjectID game_object_id) {
  objects_to_delete_.insert(game_object_id);
}

void Registry::update(const double delta_time) {
  // Update all the systems in the registry
  for (const auto& system : systems_ | std::views::values) {
    system->update(delta_time);
  }

  // Delete all marked game objects
  for (const auto game_object_id : objects_to_delete_) {
    delete_game_object(game_object_id);
  }
  objects_to_delete_.clear();
}

void Registry::fixed_update(const double delta_time) {
  // Update all the fixed-timestep systems in the registry
  for (const auto& system : systems_ | std::views::values) {
    system->fixed_update(delta_time);
  }

  // Delete all marked game objects
  for (const auto game_object_id : objects_to_delete_) {
    delete_game_object(game_object_id);
  }
  objects_to_delete_.clear();
}
