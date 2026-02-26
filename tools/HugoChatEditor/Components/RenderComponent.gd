extends Component
class_name RenderComponent

@export var meshName:String
@export var materialName:String
@export var objectID:int

func get_component() -> Dictionary[String, Variant]:
	return {
		"meshName" : meshName,
		"materialName" : materialName,
		"objectID" : objectID,
	}
