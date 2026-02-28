@tool
extends MeshInstance3D
class_name MeshRenderer

func get_component() -> Dictionary[String, Variant]:
	return {
		"mesh" : FormatWriter.format(mesh.resource_path)
	}
