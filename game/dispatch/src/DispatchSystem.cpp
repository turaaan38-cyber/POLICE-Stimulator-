#include "police/dispatch/DispatchSystem.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace police::dispatch {
namespace {
bool finite(const WorldPosition& p) {
    return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z);
}
double distance_squared(const WorldPosition& a, const WorldPosition& b) {
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    const double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}
}

const char* to_string(const CalloutKind kind) noexcept {
    switch (kind) {
    case CalloutKind::TrafficStop: return "Traffic stop";
    case CalloutKind::TrafficCollision: return "Traffic collision";
    case CalloutKind::SuspiciousPerson: return "Suspicious person";
    case CalloutKind::VehiclePursuit: return "Vehicle pursuit";
    }
    return "Unknown callout";
}

const char* to_string(const CalloutStatus status) noexcept {
    switch (status) {
    case CalloutStatus::Available: return "Available";
    case CalloutStatus::Accepted: return "Accepted";
    case CalloutStatus::OnScene: return "On scene";
    case CalloutStatus::Completed: return "Completed";
    case CalloutStatus::Cancelled: return "Cancelled";
    }
    return "Unknown status";
}

Callout* DispatchSystem::find_mutable(const std::uint64_t id) noexcept {
    for (auto& callout : callouts_) {
        if (callout.id == id) return &callout;
    }
    return nullptr;
}

const Callout* DispatchSystem::find_pointer(const std::uint64_t id) const noexcept {
    for (const auto& callout : callouts_) {
        if (callout.id == id) return &callout;
    }
    return nullptr;
}

std::uint64_t DispatchSystem::publish(CalloutDraft draft) {
    if (draft.title.empty()) throw std::invalid_argument("callout title must not be empty");
    if (draft.briefing.empty()) throw std::invalid_argument("callout briefing must not be empty");
    if (!finite(draft.location)) throw std::invalid_argument("callout position must contain finite coordinates");
    if (draft.experience_reward < 0) throw std::invalid_argument("experience reward must not be negative");
    if (next_id_ == std::numeric_limits<std::uint64_t>::max())
        throw std::overflow_error("dispatch callout id space exhausted");

    const std::uint64_t id = next_id_++;
    callouts_.push_back(Callout{id, draft.kind, std::move(draft.title), std::move(draft.briefing),
                                draft.location, draft.experience_reward, CalloutStatus::Available});
    return id;
}

bool DispatchSystem::accept(const std::uint64_t id) {
    Callout* callout = find_mutable(id);
    if (callout == nullptr || callout->status != CalloutStatus::Available || active_id_.has_value())
        return false;
    callout->status = CalloutStatus::Accepted;
    active_id_ = id;
    return true;
}

bool DispatchSystem::mark_on_scene(const std::uint64_t id, const WorldPosition player_position,
                                   const double arrival_radius) {
    Callout* callout = find_mutable(id);
    if (callout == nullptr || callout->status != CalloutStatus::Accepted || active_id_ != id)
        return false;
    if (!finite(player_position) || !std::isfinite(arrival_radius) || arrival_radius < 0.0)
        return false;
    if (distance_squared(player_position, callout->location) > arrival_radius * arrival_radius)
        return false;
    callout->status = CalloutStatus::OnScene;
    return true;
}

bool DispatchSystem::complete(const std::uint64_t id) {
    Callout* callout = find_mutable(id);
    if (callout == nullptr || callout->status != CalloutStatus::OnScene || active_id_ != id)
        return false;
    if (callout->experience_reward > 0 &&
        experience_earned_ > std::numeric_limits<std::int64_t>::max() - callout->experience_reward)
        throw std::overflow_error("experience total overflow");
    callout->status = CalloutStatus::Completed;
    experience_earned_ += callout->experience_reward;
    ++completed_count_;
    active_id_.reset();
    return true;
}

bool DispatchSystem::cancel(const std::uint64_t id) {
    Callout* callout = find_mutable(id);
    if (callout == nullptr || callout->status == CalloutStatus::Completed ||
        callout->status == CalloutStatus::Cancelled)
        return false;
    callout->status = CalloutStatus::Cancelled;
    if (active_id_ == id) active_id_.reset();
    return true;
}

std::optional<Callout> DispatchSystem::find(const std::uint64_t id) const {
    const Callout* callout = find_pointer(id);
    if (callout == nullptr) return std::nullopt;
    return *callout;
}

std::optional<Callout> DispatchSystem::active_callout() const {
    if (!active_id_) return std::nullopt;
    return find(*active_id_);
}

std::optional<Callout> DispatchSystem::next_available() const {
    for (const auto& callout : callouts_) {
        if (callout.status == CalloutStatus::Available) return callout;
    }
    return std::nullopt;
}

} // namespace police::dispatch
