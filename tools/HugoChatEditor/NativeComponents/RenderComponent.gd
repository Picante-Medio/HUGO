@tool
extends MeshInstance3D
class_name MeshRenderer

@export var materialName:String
@export var objectID:int

func get_component() -> Dictionary[String, Variant]:
	return {
		"mesh" : FormatWriter.format(mesh.resource_path),
		"materialName" : FormatWriter.format(materialName),
		"objectID" : FormatWriter.format(objectID),
	}
