#pragma once
#include <cstddef>
#include <vector>
namespace police::physics {
struct Vec3 final { double x{0.0}; double y{0.0}; double z{0.0}; };
struct Aabb final { Vec3 center{}; Vec3 half_extents{}; };
struct MoveResult final { Vec3 position{}; bool hit_x{false}; bool hit_y{false}; bool hit_z{false}; [[nodiscard]] bool collided() const noexcept { return hit_x||hit_y||hit_z; } };
// Project-owned axis-aligned kinematic collision world; not a rigid-body solver.
class PhysicsWorld final {
public:
 void add_static_box(const Aabb& box);
 void clear_static() noexcept;
 [[nodiscard]] std::size_t static_box_count() const noexcept { return static_boxes_.size(); }
 [[nodiscard]] bool overlaps(const Aabb& box) const;
 [[nodiscard]] MoveResult move_box(Vec3 center,Vec3 half_extents,Vec3 displacement) const;
private: std::vector<Aabb> static_boxes_;
};
} // namespace police::physics
