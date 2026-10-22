#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace police::dispatch {

struct WorldPosition final {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

enum class CalloutKind : std::uint8_t {
    TrafficStop,
    TrafficCollision,
    SuspiciousPerson,
    VehiclePursuit
};

enum class CalloutStatus : std::uint8_t {
    Available,
    Accepted,
    OnScene,
    Completed,
    Cancelled
};

struct CalloutDraft final {
    CalloutKind kind{CalloutKind::TrafficStop};
    std::string title;
    std::string briefing;
    WorldPosition location{};
    int experience_reward{0};
};

struct Callout final {
    std::uint64_t id{0};
    CalloutKind kind{CalloutKind::TrafficStop};
    std::string title;
    std::string briefing;
    WorldPosition location{};
    int experience_reward{0};
    CalloutStatus status{CalloutStatus::Available};
};

// Deterministic, project-owned dispatch/callout state machine.
// Exactly one callout can be accepted/in progress at a time.
class DispatchSystem final {
public:
    [[nodiscard]] std::uint64_t publish(CalloutDraft draft);
    [[nodiscard]] bool accept(std::uint64_t id);
    [[nodiscard]] bool mark_on_scene(std::uint64_t id, WorldPosition player_position,
                                     double arrival_radius);
    [[nodiscard]] bool complete(std::uint64_t id);
    [[nodiscard]] bool cancel(std::uint64_t id);

    [[nodiscard]] std::optional<Callout> find(std::uint64_t id) const;
    [[nodiscard]] std::optional<Callout> active_callout() const;
    [[nodiscard]] std::optional<Callout> next_available() const;
    [[nodiscard]] std::size_t callout_count() const noexcept { return callouts_.size(); }
    [[nodiscard]] std::uint64_t completed_count() const noexcept { return completed_count_; }
    [[nodiscard]] std::int64_t experience_earned() const noexcept { return experience_earned_; }

private:
    std::vector<Callout> callouts_;
    std::optional<std::uint64_t> active_id_;
    std::uint64_t next_id_{1};
    std::uint64_t completed_count_{0};
    std::int64_t experience_earned_{0};

    [[nodiscard]] Callout* find_mutable(std::uint64_t id) noexcept;
    [[nodiscard]] const Callout* find_pointer(std::uint64_t id) const noexcept;
};

[[nodiscard]] const char* to_string(CalloutKind kind) noexcept;
[[nodiscard]] const char* to_string(CalloutStatus status) noexcept;

} // namespace police::dispatch
