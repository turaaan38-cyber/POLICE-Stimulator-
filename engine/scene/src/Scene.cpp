#include "police/scene/Scene.hpp"
#include <stdexcept>
#include <utility>

namespace police::scene {

Entity Scene::create_entity(std::string entity_name) {
    std::uint32_t index = Entity::invalid_index;
    if (!free_indices_.empty()) {
        index = free_indices_.back();
        free_indices_.pop_back();
    } else {
        if (slots_.size() >= static_cast<std::size_t>(Entity::invalid_index))
            throw std::length_error("scene exhausted its entity index range");
        index = static_cast<std::uint32_t>(slots_.size());
        slots_.emplace_back();
    }

    Slot& slot = slots_[index];
    slot.alive = true;
    slot.name = std::move(entity_name);
    slot.has_transform = false;
    slot.transform = Transform{};
    ++alive_count_;
    return Entity{index, slot.generation};
}

bool Scene::destroy_entity(const Entity entity) noexcept {
    if (!is_alive(entity)) return false;
    Slot& slot = slots_[entity.index];
    slot.alive = false;
    slot.name.clear();
    slot.has_transform = false;
    slot.transform = Transform{};
    --alive_count_;

    // Do not wrap the generation: a slot at max generation is retired rather
    // than allowing an ancient handle to become valid after integer wraparound.
    if (slot.generation != std::numeric_limits<std::uint32_t>::max()) {
        ++slot.generation;
        free_indices_.push_back(entity.index);
    }
    return true;
}

bool Scene::is_alive(const Entity entity) const noexcept {
    return entity.valid() && static_cast<std::size_t>(entity.index) < slots_.size() &&
           slots_[entity.index].alive && slots_[entity.index].generation == entity.generation;
}

Scene::Slot& Scene::checked_slot(const Entity entity) {
    if (!is_alive(entity)) throw std::out_of_range("entity handle is invalid or stale");
    return slots_[entity.index];
}
const Scene::Slot& Scene::checked_slot(const Entity entity) const {
    if (!is_alive(entity)) throw std::out_of_range("entity handle is invalid or stale");
    return slots_[entity.index];
}

const std::string& Scene::name(const Entity entity) const { return checked_slot(entity).name; }
void Scene::set_name(const Entity entity, std::string entity_name) {
    checked_slot(entity).name = std::move(entity_name);
}

void Scene::add_transform(const Entity entity, Transform value) {
    Slot& slot = checked_slot(entity);
    if (slot.has_transform) throw std::logic_error("entity already has a Transform component");
    slot.transform = value;
    slot.has_transform = true;
}
bool Scene::remove_transform(const Entity entity) {
    Slot& slot = checked_slot(entity);
    if (!slot.has_transform) return false;
    slot.has_transform = false;
    slot.transform = Transform{};
    return true;
}
bool Scene::has_transform(const Entity entity) const { return checked_slot(entity).has_transform; }
Transform& Scene::transform(const Entity entity) {
    Slot& slot = checked_slot(entity);
    if (!slot.has_transform) throw std::logic_error("entity has no Transform component");
    return slot.transform;
}
const Transform& Scene::transform(const Entity entity) const {
    const Slot& slot = checked_slot(entity);
    if (!slot.has_transform) throw std::logic_error("entity has no Transform component");
    return slot.transform;
}

std::vector<Entity> Scene::entities() const {
    std::vector<Entity> result;
    result.reserve(alive_count_);
    for (std::size_t i = 0; i < slots_.size(); ++i) {
        const Slot& slot = slots_[i];
        if (slot.alive) result.push_back(Entity{static_cast<std::uint32_t>(i), slot.generation});
    }
    return result;
}
std::vector<Entity> Scene::entities_with_transform() const {
    std::vector<Entity> result;
    result.reserve(alive_count_);
    for (std::size_t i = 0; i < slots_.size(); ++i) {
        const Slot& slot = slots_[i];
        if (slot.alive && slot.has_transform)
            result.push_back(Entity{static_cast<std::uint32_t>(i), slot.generation});
    }
    return result;
}

} // namespace police::scene
