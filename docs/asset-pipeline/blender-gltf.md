# Blender and glTF asset workflow

## Coordinate and naming conventions

- Blender units: metric; treat one Blender unit as one meter.
- Runtime coordinates: right-handed world space, +Y up, -Z is the initial camera forward direction.
- Name source objects descriptively: `SM_` for static meshes, `VEH_` for vehicles, `COL_` for dedicated collision meshes, and `MAT_` for materials. These are conventions for future assets, not enforced importer rules yet.
- Export game-ready exchange assets as glTF 2.0 binary (`.glb`) unless an external texture layout is specifically needed.
- Prefer collision meshes simpler than visual meshes; no collision loader exists at this stage.

## Minimal manual workflow

1. Create or edit an original `.blend` file with metric units.
2. Apply scale where appropriate, check normals/materials, and name mesh/material objects.
3. Use Blender's glTF 2.0 exporter to export `.glb`.
4. Keep the `.blend` source alongside generated exports where practical.
5. Record origin, license/author, and attribution for any external asset.
6. Validate the exported file before adding runtime use. Automatic GLB validation and runtime loading are not yet implemented.

The current renderer does not consume GLB files; its temporary city and patrol-car shape are generated as code vertices. The model import milestone must add a loader, malformed-file tests, and a sample asset before this workflow is treated as end-to-end.
