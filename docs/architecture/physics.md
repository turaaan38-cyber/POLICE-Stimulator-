## First collision layer

The `engine/physics` module owns an axis-aligned bounding-box (AABB) collision world. Static colliders use a center and positive half-extents. Movement is resolved axis-by-axis and clamped against static boxes, allowing wall sliding. Invalid and non-finite coordinates are rejected.

This is kinematic collision detection/resolution, not a rigid-body solver: rotation-aware collision, dynamic-to-dynamic contacts, mass, suspension, tire friction, impulses and pedestrian capsules are not implemented. The generated blockout registers building colliders and the patrol vehicle uses a conservative fixed AABB. Vehicle collision sets its speed to zero as a simple arcade response.
