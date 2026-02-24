@icon("res://Icons/component_free_style.png")
@tool
extends MeshInstance3D
class_name MeshRenderer

var last_mesh:StringName

func _ready() -> void:
	if not mesh: last_mesh = ""
	else: last_mesh = mesh.resource_path
	
	update_mesh()

func _process(_delta: float) -> void:
	if mesh and mesh.resource_path == last_mesh: return
	if not mesh and last_mesh == "": return
	
	update_mesh()
	
func update_mesh():
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(get_parent().name, {})
	var mesh_renderer:Dictionary = entity.get_or_add("MESH_RENDERER", {})
	
	var mesh_name:String
	if not mesh: last_mesh = ""
	else: mesh_name = mesh.resource_path
	
	mesh_renderer.get_or_add("mesh", mesh_name)
	mesh_renderer["mesh"] = mesh_name
	last_mesh = mesh_name
	
	print(get_parent().name + " mesh updated to " + last_mesh)

func _exit_tree() -> void:
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(get_parent().name, {})
	entity.erase("MESH_RENDERER")
