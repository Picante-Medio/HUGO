extends Component
class_name LightComponent

@export var diffuse:Vector4
@export var specular:Vector4

func get_component() -> Dictionary[String, Variant]:
	return {
		"diffuse" : FormatWriter.format(diffuse),
		"specular" : FormatWriter.format(specular),
	}
