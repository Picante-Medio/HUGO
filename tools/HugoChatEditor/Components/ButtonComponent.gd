@tool
extends Component
class_name ButtonComponent

@export var normalMaterial:String
@export var hoverMaterial:String
@export var pressedMaterial:String
@export var disabledMaterial:String

func get_component() -> Dictionary[String, Variant]:
	return {
		"normalMaterial" : FormatWriter.format(normalMaterial, "normalMaterial"),
		"hoverMaterial" : FormatWriter.format(hoverMaterial, "hoverMaterial"),
		"pressedMaterial" : FormatWriter.format(pressedMaterial, "pressedMaterial"),
		"disabledMaterial" : FormatWriter.format(disabledMaterial, "disabledMaterial"),
	}
