@tool
extends Component
class_name LightComponent

@export var diffuse:Vector4
@export var specular:Vector4
@export var spotFalloff:float
@export var spotNearClip:float
@export var attenuation:Vector4
@export var shadowFarDist:float
@export var shadowFarDistSquared:float
@export var shadowNearClipDist:float
@export var shadowFarClipDist:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"diffuse" : FormatWriter.format(diffuse),
		"specular" : FormatWriter.format(specular),
		"spotFalloff" : FormatWriter.format(spotFalloff),
		"spotNearClip" : FormatWriter.format(spotNearClip),
		"attenuation" : FormatWriter.format(attenuation),
		"shadowFarDist" : FormatWriter.format(shadowFarDist),
		"shadowFarDistSquared" : FormatWriter.format(shadowFarDistSquared),
		"shadowNearClipDist" : FormatWriter.format(shadowNearClipDist),
		"shadowFarClipDist" : FormatWriter.format(shadowFarClipDist),
	}
