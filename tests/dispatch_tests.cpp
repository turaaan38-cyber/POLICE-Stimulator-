#include "police/dispatch/DispatchSystem.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
int failures = 0;
void check(bool passed, const char* label) {
    if (passed) std::cout << "[PASS] " << label << '\n';
    else { std::cerr << "[FAIL] " << label << '\n'; ++failures; }
}
template<class F>
void expect_invalid(F&& action, const char* label) {
    try { action(); check(false, label); }
    catch (const std::invalid_argument&) { check(true, label); }
    catch (...) { check(false, label); }
}
police::dispatch::CalloutDraft draft(const char* title, double x, int reward) {
    using namespace police::dispatch;
    return {CalloutKind::TrafficStop, title, "A vehicle requires a traffic stop.", {x, 0.0, 0.0}, reward};
}
}
int main() {
    using namespace police::dispatch;
    DispatchSystem dispatch;
    expect_invalid([&] { (void)dispatch.publish(draft("", 0.0, 10)); }, "rejects an empty callout title");
    expect_invalid([&] { auto d=draft("Bad location", 0.0, 10); d.location.y=std::numeric_limits<double>::infinity(); (void)dispatch.publish(d); }, "rejects non-finite location");
    expect_invalid([&] { (void)dispatch.publish(draft("Negative reward", 0.0, -1)); }, "rejects negative experience reward");

    const auto id = dispatch.publish(draft("Traffic stop", 10.0, 100));
    const auto second = dispatch.publish(draft("Collision", 25.0, 250));
    check(id == 1 && second == 2, "assigns stable monotonic callout ids");
    check(dispatch.callout_count() == 2, "tracks published callout count");
    check(dispatch.next_available()->id == id, "returns the first available callout");
    check(!dispatch.accept(999), "rejects an unknown callout id");
    check(!dispatch.mark_on_scene(id, {10.0,0.0,0.0}, 5.0), "cannot arrive before accepting");
    check(!dispatch.complete(id), "cannot complete an unaccepted callout");
    check(dispatch.accept(id), "accepts an available callout");
    check(!dispatch.accept(second), "allows only one active callout at a time");
    check(dispatch.active_callout()->id == id, "exposes active callout state");
    check(!dispatch.mark_on_scene(id, {20.0,0.0,0.0}, 5.0), "rejects arrival outside radius");
    check(!dispatch.mark_on_scene(id, {10.0,0.0,0.0}, -1.0), "rejects a negative arrival radius");
    check(dispatch.mark_on_scene(id, {14.0,0.0,0.0}, 5.0), "marks arrival within radius");
    check(!dispatch.accept(id), "cannot accept an already active callout twice");
    check(!dispatch.cancel(id) == false, "can cancel an active callout");

    check(dispatch.accept(second), "next callout becomes available after cancellation");
    check(dispatch.mark_on_scene(second, {25.0,0.0,0.0}, 1.0), "can arrive at second callout");
    check(dispatch.complete(second), "completes a callout on scene");
    check(dispatch.completed_count() == 1, "counts completed callouts");
    check(dispatch.experience_earned() == 250, "awards experience once on completion");
    check(!dispatch.complete(second), "prevents duplicate completion and reward");
    check(!dispatch.cancel(second), "completed callout cannot be cancelled");
    check(!dispatch.active_callout().has_value(), "clears active callout after completion");
    check(dispatch.find(id)->status == CalloutStatus::Cancelled, "cancelled state is retained in callout history");
    if (failures != 0) {
        std::cerr << failures << " dispatch test(s) failed\n";
        return 1;
    }
    std::cout << "All dispatch tests passed\n";
    return 0;
}
