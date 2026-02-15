// Ensure this file is only included once
#pragma once

// Std headers
#include <algorithm>
#include <cassert>
#include <memory>
#include <ranges>
#include <vector>

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
namespace exodus::ecs {
/// Represents unique identifiers for game objects
using GameObjectID = std::uint32_t;

/// Manages game objects and their components in the registry.
class Registry {
 public:
  /// Create a new game object in the registry.
  ///
  /// @return The ID of the newly created game object.
  auto create() -> GameObjectID {
    const GameObjectID game_object_id{next_game_object_id_++};
    alive_.push_back(true);
    return game_object_id;
  }

  /// Check if a game object with the given ID exists in the registry.
  ///
  /// @param game_object_id The ID of the game object to check for.
  /// @return True if the game object exists in the registry, false otherwise.
  [[nodiscard]] auto has(const GameObjectID game_object_id) const noexcept -> bool {
    return game_object_id < alive_.size() && alive_[game_object_id];
  }

  /// Destroy the game object with the given ID, removing all its components from the registry.
  ///
  /// @param game_object_id The ID of the game object to destroy.
  void destroy(const GameObjectID game_object_id) {
    if (!has(game_object_id)) {
      return;
    }
    for (const auto& storage : storages_) {
      if (storage) {
        storage->remove(game_object_id);
      }
    }
    alive_[game_object_id] = false;
  }

  /// Get the total number of alive game objects in the registry.
  ///
  /// @return The total number of alive game objects in the registry.
  [[nodiscard]] auto count() const noexcept -> std::size_t { return std::ranges::count(alive_, true); }

  /// Add a component of a certain type to a game object in the registry ignoring if it already exists.
  ///
  /// @tparam Component The type of component to add.
  /// @tparam Args The types of arguments to pass to the component constructor.
  /// @param game_object_id The ID of the game object to add the component to.
  /// @param args The arguments to pass to the component constructor.
  template <typename Component, typename... Args>
  void add_component(const GameObjectID game_object_id, Args&&... args) {
    // Make sure the storage has enough size for this game object
    assert(has(game_object_id));
    auto& storage{get_storage<Component>()};
    if (game_object_id >= storage.sparse.size()) {
      storage.sparse.resize(game_object_id + 1, INVALID_COMPONENT_INDEX);
    }

    // Stop if we already have this component
    if (storage.sparse[game_object_id] != INVALID_COMPONENT_INDEX) {
      return;
    }

    // Add the component to the end of the storage
    storage.components.emplace_back(std::forward<Args>(args)...);
    storage.game_objects.emplace_back(game_object_id);
    storage.sparse[game_object_id] = storage.components.size() - 1;
  }

  /// Get a component from the registry for a given game object ID.
  ///
  /// @tparam Component The type of component to get.
  /// @param game_object_id The ID of the game object to get the component for.
  /// @return A reference to the component of the given type for the specified game object ID.
  template <typename Component>
  auto get_component(const GameObjectID game_object_id) -> Component& {
    assert(has(game_object_id));
    auto& storage{get_storage<Component>()};
    assert(game_object_id < storage.sparse.size());
    const auto index{storage.sparse[game_object_id]};
    assert(index != INVALID_COMPONENT_INDEX);
    return storage.components[index];
  }

  /// Check if a game object has a component of a certain type.
  ///
  /// @tparam Component The type of component to check for.
  /// @param game_object_id The ID of the game object to check for the component.
  /// @return True if the game object has a component of the specified type, false otherwise
  template <typename Component>
  [[nodiscard]] auto has_component(const GameObjectID game_object_id) const -> bool {
    // Check if the game object exists and if the component storage exists
    if (!has(game_object_id)) {
      return false;
    }
    const std::size_t type_id{component_type_id<Component>()};
    if (type_id >= storages_.size() || !storages_[type_id]) {
      return false;
    }

    // Check if the game object has the component in the storage
    const auto* storage{static_cast<Storage<Component>*>(storages_[type_id].get())};
    return game_object_id < storage->sparse.size() && storage->sparse[game_object_id] != INVALID_COMPONENT_INDEX;
  }

  /// Create a view for iterating over game objects that have a specific set of components.
  ///
  /// @tparam Component The types of components to include in the view.
  /// @return A view that allows iterating over game objects that have the specified components.
  template <typename... Component>
  auto view() {
    // Get all the storages for the given components
    static_assert(sizeof...(Component) > 0, "Cannot create a view with no components");
    auto storages{std::tie(get_storage<Component>()...)};

    // Get the primary storage which will act as the first component to iterate over. This should be the component with
    // the least number of instances to minimise the number of iterations needed
    auto& primary{std::get<0>(storages)};

    // Create a lazy generator using a range
    auto rng{std::views::iota(std::size_t{0}, primary.components.size()) |
             std::views::filter([&](const std::size_t val) -> auto {
               return (has_component<Component>(primary.game_objects[val]) && ...);
             }) |
             std::views::transform([&](std::size_t val) -> std::tuple<Component&...> {
               return std::tuple<Component&...>{get_component<Component>(primary.game_objects[val])...};
             })};
    return rng;
  }

 private:
  /// A constant representing an invalid component index in the sparse array.
  inline static std::size_t INVALID_COMPONENT_INDEX{static_cast<std::size_t>(-1)};

  /// An interface for component storage, allowing for type-erased storage of different component types.
  struct IStorage {
    /// Create the storage.
    IStorage() = default;

    /// Destroy the storage.
    virtual ~IStorage() = default;

    /// Deleted copy constructor to prevent object slicing.
    IStorage(const IStorage&) = delete;

    /// Deleted copy assignment to prevent object slicing.
    auto operator=(const IStorage&) -> IStorage& = delete;

    /// Deleted move constructor to prevent object slicing.
    IStorage(IStorage&&) = delete;

    /// Deleted move assignment to prevent object slicing.
    auto operator=(IStorage&&) -> IStorage& = delete;

    /// Remove the component associated with the given game object ID from the storage.
    virtual void remove(GameObjectID game_object_id) = 0;
  };

  /// A dense bidirectional storage for components of a specific type, implemented as a sparse set.
  ///
  /// Storage model:
  /// - Each component type has its own `Storage`, indexed by a compile-time assigned type ID.
  /// - Components are stored densely in `components` where `game_objects[i]` is the owner of `components[i]`.
  /// - `sparse[g]` maps a game object ID `g` to its component’s dense index, or `INVALID_COMPONENT_INDEX` if the
  ///   component is absent.
  ///
  /// Behaviour:
  /// - Adding a component appends it to the dense arrays and records its index in `sparse`.
  /// - Removing a component fills the gap by moving the last dense element into the removed slot and updating the
  ///   affected sparse entry.
  template <typename Component>
  struct Storage : IStorage {
    /// Stores dense component data, indexed by dense indices.
    std::vector<Component> components;

    /// A dense mapping from dense index to owning game object IDs.
    std::vector<GameObjectID> game_objects;

    /// A sparse mapping from game object ID to dense indices.
    std::vector<size_t> sparse;

    /// Remove the component associated with the given game object ID from the storage.
    void remove(const GameObjectID game_object_id) override {
      // Check if the game object ID is valid and has a component in the storage
      if (game_object_id >= sparse.size()) {
        return;
      }
      const size_t index{sparse[game_object_id]};
      if (index == INVALID_COMPONENT_INDEX) {
        return;
      }

      // Move the component to the end if it is not the last one in the storage to maintain the sparse set structure
      if (const size_t last{components.size() - 1}; index != last) {
        components[index] = std::move(components[last]);
        game_objects[index] = game_objects[last];
        sparse[game_objects[index]] = index;
      }

      // Remove the last element from the storage
      components.pop_back();
      game_objects.pop_back();
      sparse[game_object_id] = INVALID_COMPONENT_INDEX;
    }
  };

  /// The next game object ID to use.
  GameObjectID next_game_object_id_{0};

  /// The next component type ID to use.
  inline static std::size_t next_component_type_id_{0};

  /// Get the component type ID for a given component type.
  ///
  /// @return The component type ID for the given component type.
  template <typename Component>
  static auto component_type_id() -> std::size_t {
    (void)typeid(Component);  // Ignore unused template warning
    static std::size_t const component_type_id{next_component_type_id_++};
    return component_type_id;
  }

  /// A vector of storages for each component type, indexed by component type ID.
  std::vector<std::unique_ptr<IStorage>> storages_;

  /// Get the storage for a given component type, creating it if it does not exist.
  ///
  /// @tparam Component The type of component to get the storage for.
  /// @return The storage for the given component type.
  template <typename Component>
  auto get_storage() -> Storage<Component>& {
    // Resize storages vector if needed
    const std::size_t type_id{component_type_id<Component>()};
    if (type_id >= storages_.size()) {
      storages_.resize(type_id + 1);
    }

    // Lazily create storage for this component type
    if (!storages_[type_id]) {
      storages_[type_id] = std::make_unique<Storage<Component>>();
    }
    return *static_cast<Storage<Component>*>(storages_[type_id].get());
  }

  /// A vector of booleans indicating whether each game object ID is alive or not, indexed by game object ID.
  std::vector<bool> alive_;
};
}  // namespace exodus::ecs
// NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
