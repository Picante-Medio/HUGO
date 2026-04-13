@tool
extends Component
class_name LightComponent

@export var diffuse:Vector4
@export var specular:Vector4
@export var lightType:ENUMS.LightType
@export var spotFalloff:float
@export var spotNearClip:float
@export var spotOuter:float
@export var spotInner:float
@export var shadowFarDist:float
@export var shadowFarDistSquared:float
@export var attenuation:Vector4
@export var shadowNearClipDist:float
@export var shadowFarClipDist:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"diffuse" : FormatWriter.format(diffuse, "diffuse"),
		"specular" : FormatWriter.format(specular, "specular"),
		"lightType" : FormatWriter.format(lightType, "lightType"),
		"spotFalloff" : FormatWriter.format(spotFalloff, "spotFalloff"),
		"spotNearClip" : FormatWriter.format(spotNearClip, "spotNearClip"),
		"spotOuter" : FormatWriter.format(spotOuter, "spotOuter"),
		"spotInner" : FormatWriter.format(spotInner, "spotInner"),
		"shadowFarDist" : FormatWriter.format(shadowFarDist, "shadowFarDist"),
		"shadowFarDistSquared" : FormatWriter.format(shadowFarDistSquared, "shadowFarDistSquared"),
		"attenuation" : FormatWriter.format(attenuation, "attenuation"),
		"shadowNearClipDist" : FormatWriter.format(shadowNearClipDist, "shadowNearClipDist"),
		"shadowFarClipDist" : FormatWriter.format(shadowFarClipDist, "shadowFarClipDist"),
	}
