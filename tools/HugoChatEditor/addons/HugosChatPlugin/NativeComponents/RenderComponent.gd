@tool
extends MeshInstance3D
class_name RenderComponent

@export var materialName:String
@export var objectID:int

func get_component() -> Dictionary[String, Variant]:
	var splitted_resource_path:PackedStringArray = mesh.resource_path.get_basename().split("/")
	var mesh_name = splitted_resource_path[splitted_resource_path.size() - 1]
	print(mesh_name)
	return {
		"mesh" : FormatWriter.format(mesh.resource_path, "mesh"),
		"materialName" : FormatWriter.format(materialName, "materialName"),
		"objectID" : FormatWriter.format(objectID, "objectID"),
	}
