@tool
extends MeshInstance3D
class_name MeshRenderer

func get_component() -> Dictionary[String, Variant]:
	return {
		"mesh" : mesh.resource_path
	}
