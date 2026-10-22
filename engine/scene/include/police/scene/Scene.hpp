#pragma once
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace police::scene {

struct Entity final {
    static constexpr std::uint32_t invalid_index = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t index{invalid_index};
    std::uint32_t generation{0};
    [[nodiscard]] constexpr bool valid() const noexcept { return index != invalid_index && generation != 0; }
    friend constexpr bool operator==(Entity, Entity) noexcept = default;
};

struct Vec3 final { double x{0.0}; double y{0.0}; double z{0.0}; };
struct Transform final {
    Vec3 position{};
    Vec3 rotation_degrees{};
    Vec3 scale{1.0, 1.0, 1.0};
};

// Minimal project-owned entity/transform scene. Generational handles prevent a
// destroyed entity reference from silently addressing a later entity reusing its slot.
class Scene final {
public:
    [[nodiscard]] Entity create_entity(std::string name = "Entity");
    [[nodiscard]] bool destroy_entity(Entity entity) noexcept;
    [[nodiscard]] bool is_alive(Entity entity) const noexcept;
    [[nodiscard]] std::size_t entity_count() const noexcept { return alive_count_; }

    [[nodiscard]] const std::string& name(Entity entity) const;
    void set_name(Entity entity, std::string name);

    void add_transform(Entity entity, Transform transform = {});
    [[nodiscard]] bool remove_transform(Entity entity);
    [[nodiscard]] bool has_transform(Entity entity) const;
    [[nodiscard]] Transform& transform(Entity entity);
    [[nodiscard]] const Transform& transform(Entity entity) const;

    [[nodiscard]] std::vector<Entity> entities() const;
    [[nodiscard]] std::vector<Entity> entities_with_transform() const;

private:
    struct Slot final {
        std::uint32_t generation{1};
        bool alive{false};
        std::string name;
        bool has_transform{false};
        Transform transform{};
    };

    std::vector<Slot> slots_;
    std::vector<std::uint32_t> free_indices_;
    std::size_t alive_count_{0};

    [[nodiscard]] Slot& checked_slot(Entity entity);
    [[nodiscard]] const Slot& checked_slot(Entity entity) const;
};

} // namespace police::scene
