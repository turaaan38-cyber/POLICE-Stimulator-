#include "police/scene/Scene.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
int failures = 0;
void check(bool passed, const char* label) {
    if (passed) std::cout << "[PASS] " << label << '\n';
    else { std::cerr << "[FAIL] " << label << '\n'; ++failures; }
}
template<class F> void expect_exception(F&& action, const char* label) {
    try { action(); check(false, label); }
    catch (const std::out_of_range&) { check(true, label); }
    catch (const std::logic_error&) { check(true, label); }
    catch (...) { check(false, label); }
}
}
int main() {
    using namespace police::scene;
    Scene scene;
    const Entity car = scene.create_entity("VEH_Patrol");
    const Entity building = scene.create_entity("BLD_Test");
    check(scene.entity_count() == 2, "entity count increases on creation");
    check(scene.is_alive(car) && scene.name(car) == "VEH_Patrol", "valid handle and name are readable");
    check(!scene.has_transform(car), "transform is absent until added");

    Transform pose;
    pose.position = {12.0, 0.0, -4.0};
    pose.rotation_degrees.y = 37.0;
    pose.scale = {1.0, 1.0, 1.0};
    scene.add_transform(car, pose);
    check(scene.has_transform(car), "Transform component can be attached");
    check(std::abs(scene.transform(car).position.x - 12.0) < 1e-12, "transform position persists");
    scene.transform(car).position.z = -9.0;
    check(std::abs(scene.transform(car).position.z + 9.0) < 1e-12, "transform reference updates scene data");
    expect_exception([&] { scene.add_transform(car); }, "duplicate Transform component is rejected");
    expect_exception([&] { (void)scene.transform(building); }, "missing Transform component is diagnosed");

    const auto all = scene.entities();
    const auto moved = scene.entities_with_transform();
    check(all.size() == 2, "entity query returns all live entities");
    check(moved.size() == 1 && moved.front() == car, "component query returns only transformed entities");
    check(scene.remove_transform(car) && !scene.has_transform(car), "Transform component can be removed");
    check(!scene.remove_transform(car), "removing an absent component reports false");

    check(scene.destroy_entity(car), "entity destruction succeeds once");
    check(!scene.destroy_entity(car), "double destruction is rejected");
    check(!scene.is_alive(car) && scene.entity_count() == 1, "destroyed entity handle becomes stale");
    expect_exception([&] { (void)scene.name(car); }, "stale handle cannot read another entity");

    const Entity replacement = scene.create_entity("VEH_Replacement");
    check(replacement.index == car.index && replacement.generation != car.generation,
          "reused slot receives a new generation");
    check(!scene.is_alive(car) && scene.is_alive(replacement), "old handle cannot alias a reused slot");
    check(!scene.has_transform(replacement), "reused slot does not retain prior components");
    scene.set_name(replacement, "VEH_Unit_01");
    check(scene.name(replacement) == "VEH_Unit_01", "entity name can be changed");
    if (failures != 0) {
        std::cerr << failures << " scene test(s) failed\n";
        return 1;
    }
    std::cout << "All scene tests passed\n";
    return 0;
}
