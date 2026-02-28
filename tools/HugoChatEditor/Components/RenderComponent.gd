extends Component
class_name RenderComponent

@export var meshName:String
@export var materialName:String
@export var objectID:int

func get_component() -> Dictionary[String, Variant]:
	return {
		"meshName" : FormatWriter.format(meshName),
		"materialName" : FormatWriter.format(materialName),
		"objectID" : FormatWriter.format(objectID),
	}
