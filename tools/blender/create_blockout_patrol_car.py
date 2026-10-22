"""Create original low-poly patrol-car placeholder and export GLB.
Run: blender -b --python tools/blender/create_blockout_patrol_car.py -- --output assets/models/patrol_car_blockout.glb
Runtime GLB import is not implemented yet.
"""
import argparse, os, sys
import bpy
def parse_args():
    args=sys.argv[sys.argv.index("--")+1:] if "--" in sys.argv else []
    p=argparse.ArgumentParser();p.add_argument("--output",default="assets/models/patrol_car_blockout.glb");return p.parse_args(args)
def material(name,color,metallic=0.0,roughness=0.55):
    m=bpy.data.materials.new(name);m.diffuse_color=(*color,1.0);m.use_nodes=True
    n=m.node_tree.nodes.get("Principled BSDF")
    if n:n.inputs["Base Color"].default_value=(*color,1.0);n.inputs["Metallic"].default_value=metallic;n.inputs["Roughness"].default_value=roughness
    return m
def cube(name,loc,scale,mat):
    bpy.ops.mesh.primitive_cube_add(size=1.0,location=loc);o=bpy.context.object;o.name=name;o.dimensions=scale
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(mat)
    b=o.modifiers.new("Soft low-poly edges","BEVEL");b.width=.035;b.segments=1;o.modifiers.new("Weighted corner normals","WEIGHTED_NORMAL");return o
def wheel(name,loc,mat):
    bpy.ops.mesh.primitive_cylinder_add(vertices=12,radius=.34,depth=.18,end_fill_type="NGON",location=loc,rotation=(0.0,1.57079632679,0.0))
    o=bpy.context.object;o.name=name;o.data.materials.append(mat)
    for f in o.data.polygons:f.use_smooth=False
def main():
    opt=parse_args();out=os.path.abspath(opt.output);os.makedirs(os.path.dirname(out),exist_ok=True)
    bpy.ops.object.select_all(action="SELECT");bpy.ops.object.delete(use_global=False)
    for coll in (bpy.data.meshes,bpy.data.materials,bpy.data.curves):
        for d in list(coll):
            if d.users==0:coll.remove(d)
    bpy.context.scene.unit_settings.system="METRIC";bpy.context.scene.unit_settings.scale_length=1.0
    white=material("MAT_Patrol_White",(.82,.84,.84),.18,.35);navy=material("MAT_Patrol_Navy",(.035,.10,.16),.22,.36)
    glass=material("MAT_Glass_Blue",(.035,.22,.30),.12,.24);rubber=material("MAT_Rubber",(.025,.028,.03),0,.84)
    red=material("MAT_Light_Red",(.85,.02,.025),.05,.22);blue=material("MAT_Light_Blue",(.015,.08,.95),.05,.22);lamp=material("MAT_Lamp",(.92,.85,.60),0,.24)
    cube("VEH_Patrol_Body",(0,.58,0),(1.82,.72,4.15),white);cube("VEH_Patrol_Hood",(0,.95,-1.22),(1.72,.22,1.34),white)
    cube("VEH_Patrol_Cabin",(0,1.18,.20),(1.53,.90,1.84),navy);cube("VEH_Patrol_Windshield",(0,1.42,-.52),(1.36,.52,.05),glass)
    cube("VEH_Patrol_RearGlass",(0,1.42,.98),(1.34,.48,.05),glass);cube("VEH_Patrol_Roof",(0,1.66,.20),(1.48,.12,1.78),white)
    cube("VEH_Patrol_Lightbar",(0,1.79,.08),(.78,.12,.22),navy);cube("VEH_Patrol_RedLight",(-.22,1.86,.08),(.25,.08,.18),red);cube("VEH_Patrol_BlueLight",(.22,1.86,.08),(.25,.08,.18),blue)
    for side in (-1,1):
        for axle in (-1,1):wheel(f"VEH_Patrol_Wheel_{side}_{axle}",(side*.91,.39,axle*1.34),rubber)
        cube(f"VEH_Patrol_SideStripe_{side}",(side*.918,.66,.05),(.025,.14,1.38),navy)
    cube("VEH_Patrol_Headlamp_L",(-.55,.70,-2.09),(.32,.18,.05),lamp);cube("VEH_Patrol_Headlamp_R",(.55,.70,-2.09),(.32,.18,.05),lamp)
    bpy.ops.wm.save_as_mainfile(filepath=os.path.splitext(out)[0]+".blend")
    bpy.ops.object.select_all(action="SELECT");bpy.ops.export_scene.gltf(filepath=out,export_format="GLB",use_selection=True)
    print(f"Created Blender source and GLB: {out}")
if __name__=="__main__":main()
