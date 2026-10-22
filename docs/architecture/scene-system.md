# Scene and entity lifecycle (first iteration)

The `police_scene` library is an in-project scene foundation with generational entity handles, names, and optional Transform components. Destroying an entity invalidates its handle. If a slot is reused, the generation changes, so old handles cannot silently refer to a replacement. Invalid, stale, duplicate, and missing-component operations are diagnosed.

The renderer now creates a named patrol-car entity and synchronizes its Transform with the vehicle controller each update; that Transform is used for the car's render placement and follow camera. Static city geometry remains a procedural batch for now.

This is an initial entity/Transform component model, not a complete generic ECS: arbitrary component registration, hierarchy/parenting, serialization, scene streaming, and editor inspection have not been implemented.
