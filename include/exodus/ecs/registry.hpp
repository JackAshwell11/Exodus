// Ensure this file is only included once
#pragma once

// Std headers
#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
namespace exodus::ecs {
class Registry;

/// Represents unique identifiers for game objects
using GameObjectID = std::uint32_t;

/// A view that allows iterating over game objects that have a specific set of components.
template <typename... Component>
class View {
 public:
  /// An iterator for iterating over the game objects in the view.
  struct iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Component&...>;

    /// The registry which manages the game objects and their components.
    Registry* registry{};

    /// A pointer to the game objects which are included in this view.
    std::span<const GameObjectID> game_objects;

    /// The current index of the iterator in the game objects vector.
    std::size_t index{0};

    /// Dereference the iterator to get a tuple of references to the components of the current game object.
    ///
    /// @return A tuple of references to the components of the current game object.
    auto operator*() const;

    /// Increment the iterator to move to the next game object in the view.
    ///
    /// @return A reference to the incremented iterator.
    auto operator++() -> iterator& {
      ++index;
      return *this;
    }

    /// Post-increment the iterator to move to the next game object in the view.
    ///
    /// @return A copy of the iterator before incrementing.
    auto operator++(int) -> iterator {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }

    /// Check if two iterators are equal by comparing their indices.
    ///
    /// @param other The other iterator to compare with.
    auto operator==(const iterator& other) const -> bool { return index == other.index; }

    /// Check if two iterators are not equal by comparing their indices.
    ///
    /// @param other The other iterator to compare with.
    auto operator!=(const iterator& other) const -> bool { return index != other.index; }
  };

  /// Construct a view.
  ///
  /// @param registry The registry which manages the game objects and their components.
  /// @param game_objects The game objects which are included in this view.
  View(Registry* registry, const std::span<const GameObjectID>& game_objects)
      : registry{registry}, game_objects{game_objects} {}

  /// Get an iterator to the beginning of the view.
  ///
  /// @return An iterator to the beginning of the view.
  auto begin() -> iterator { return {registry, game_objects, 0}; }

  /// Get an iterator to the end of the view.
  ///
  /// @return An iterator to the end of the view.
  auto end() -> iterator { return {registry, game_objects, game_objects.size()}; }

 private:
  /// The registry which manages the game objects and their components.
  Registry* registry{};

  /// The game objects which are included in this view.
  std::span<const GameObjectID> game_objects;
};

/// Manages game objects and their components in the registry.
class Registry {
 public:
  /// Create a new game object in the registry.
  ///
  /// @return The ID of the newly created game object.
  auto create() -> GameObjectID {
    const GameObjectID game_object_id{next_game_object_id_++};
    alive_.push_back(true);
    dirty_views();
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
    dirty_views();
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
    auto& storage{get_storage<Component>()};
    if (game_object_id >= storage.sparse.size()) {
      storage.sparse.resize(game_object_id + 1, INVALID_INDEX);
    }

    // Stop if we already have this component
    if (storage.sparse[game_object_id] != INVALID_INDEX) {
      return;
    }

    // Add the component to the end of the storage
    storage.components.emplace_back(std::forward<Args>(args)...);
    storage.game_objects.emplace_back(game_object_id);
    storage.sparse[game_object_id] = storage.components.size() - 1;
    dirty_views(type_id<Component, ComponentTag>());
  }

  /// Get a component from the registry for a given game object ID.
  ///
  /// @tparam Component The type of component to get.
  /// @param game_object_id The ID of the game object to get the component for.
  /// @return A reference to the component of the given type for the specified game object ID.
  template <typename Component>
  auto get_component(const GameObjectID game_object_id) -> Component& {
    auto& storage{get_storage<Component>()};
    const auto index{storage.sparse[game_object_id]};
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
    const std::size_t component_type_id{type_id<Component, ComponentTag>()};
    if (component_type_id >= storages_.size() || !storages_[component_type_id]) {
      return false;
    }

    // Check if the game object has the component in the storage
    const auto* storage{static_cast<Storage<Component>*>(storages_[component_type_id].get())};
    return game_object_id < storage->sparse.size() && storage->sparse[game_object_id] != INVALID_INDEX;
  }

  /// Create a view for iterating over game objects that have a specific set of components.
  ///
  /// @tparam Primary The type of the primary component to iterate over, which should be the component with the least
  /// number of instances.
  /// @tparam Rest The types of the other components to iterate over.
  /// @return A view that allows iterating over game objects that have the specified components.
  template <typename Primary, typename... Rest>
  auto view() -> View<Primary, Rest...> {
    // Resize the view cache if needed
    const std::size_t view_type_id{type_id<std::tuple<Primary, Rest...>, ViewTag>()};
    if (view_type_id >= view_cache_.size()) {
      view_cache_.resize(view_type_id + 1);
    }
    auto& [game_objects, component_type_ids, dirty]{view_cache_[view_type_id]};
    if (component_type_ids.empty()) {
      component_type_ids = {type_id<Primary, ComponentTag>(), type_id<Rest, ComponentTag>()...};
    }

    // If it's dirty, recompute the game objects for this view
    if (dirty) {
      auto& primary{get_storage<Primary>()};
      if constexpr (sizeof...(Rest) == 0) {
        game_objects = std::vector<GameObjectID>(primary.game_objects.begin(), primary.game_objects.end());
      } else {
        auto filtered{primary.game_objects | std::views::filter([this](const GameObjectID game_object_id) {
                        return (has_component<Rest>(game_object_id) && ...);
                      })};
        game_objects = std::vector<GameObjectID>(filtered.begin(), filtered.end());
      }
      dirty = false;
    }
    return View<Primary, Rest...>{this, game_objects};
  }

 private:
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
  /// - `sparse[g]` maps a game object ID `g` to its component’s dense index, or `INVALID_INDEX` if the component is
  ///   absent.
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
      if (index == INVALID_INDEX) {
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
      sparse[game_object_id] = INVALID_INDEX;
    }
  };

  /// A cache entry for a view, storing the cached game objects that match the component combination for the view.
  struct ViewCacheEntry {
    /// The game objects that match the component combination for this cache entry.
    std::vector<GameObjectID> objects;

    /// The component type IDs that this cache entry depends on, used for dirtying the cache when components are added
    /// or removed.
    std::vector<std::size_t> component_type_ids;

    /// A flag indicating whether the cache entry is dirty and needs to be updated.
    bool dirty{true};
  };

  /// A tag struct used to identify component types in the registry.
  struct ComponentTag {};

  /// A tag struct used to identify view types in the registry.
  struct ViewTag {};

  /// Get the component type ID for a given component type.
  ///
  /// @return The component type ID for the given component type.
  template <typename T, typename Tag>
  static auto type_id() -> std::size_t {
    (void)typeid(T);  // Ignore unused template warning
    if constexpr (std::is_same_v<Tag, ComponentTag>) {
      static std::size_t const component_type_id{next_component_type_id_++};
      return component_type_id;
    } else if constexpr (std::is_same_v<Tag, ViewTag>) {
      static std::size_t const view_type_id{next_view_type_id_++};
      return view_type_id;
    } else {
      static_assert(std::is_same_v<Tag, ComponentTag> || std::is_same_v<Tag, ViewTag>, "Invalid tag type for type_id");
      return -1;
    }
  }

  /// Get the storage for a given component type, creating it if it does not exist.
  ///
  /// @tparam Component The type of component to get the storage for.
  /// @return The storage for the given component type.
  template <typename Component>
  auto get_storage() -> Storage<Component>& {
    // Resize storages vector if needed
    const std::size_t component_type_id{type_id<Component, ComponentTag>()};
    if (component_type_id >= storages_.size()) {
      storages_.resize(component_type_id + 1);
    }

    // Lazily create storage for this component type
    if (!storages_[component_type_id]) {
      storages_[component_type_id] = std::make_unique<Storage<Component>>();
    }
    return *static_cast<Storage<Component>*>(storages_[component_type_id].get());
  }

  /// Mark cached views as dirty, so they will be rebuilt on next access.
  ///
  /// @param component_type_id The component type ID that was added or removed or `INVALID_INDEX` to dirty all views.
  void dirty_views(const std::size_t component_type_id = INVALID_INDEX) {
    for (auto& [game_objects, component_type_ids, dirty] : view_cache_) {
      if (component_type_id == INVALID_INDEX ||
          std::ranges::find(component_type_ids, component_type_id) != component_type_ids.end()) {
        dirty = true;
      }
    }
  }

  /// A constant representing an invalid index for views, components, and type IDs.
  inline static std::size_t INVALID_INDEX{static_cast<std::size_t>(-1)};

  /// The next component type ID to use.
  inline static std::size_t next_component_type_id_{0};

  /// The next view type ID to use.
  inline static std::size_t next_view_type_id_{0};

  /// The next game object ID to use.
  GameObjectID next_game_object_id_{0};

  /// A vector of storages for each component type, indexed by the component type ID.
  std::vector<std::unique_ptr<IStorage>> storages_;

  /// A vector of view cache entries, indexed by the view ID.
  std::vector<ViewCacheEntry> view_cache_;

  /// A vector of booleans indicating whether each game object ID is alive or not, indexed by game object ID.
  std::vector<bool> alive_;
};

template <typename... Component>
auto View<Component...>::iterator::operator*() const {
  const GameObjectID game_object_id{game_objects[index]};
  return std::tuple<Component&...>{registry->get_component<Component>(game_object_id)...};
}
}  // namespace exodus::ecs
// NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
