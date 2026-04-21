@tool
extends Component
class_name InputFieldComponent

@export var isActive:bool
@export var originalText:String

func get_component() -> Dictionary[String, Variant]:
	return {
		"isActive" : FormatWriter.format(isActive, "isActive"),
		"originalText" : FormatWriter.format(originalText, "originalText"),
	}
