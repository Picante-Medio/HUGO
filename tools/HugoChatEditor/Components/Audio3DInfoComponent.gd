@tool
extends Component
class_name Audio3DInfoComponent

@export var minDistance:float
@export var maxDistance:float
@export var velocity:Vector3
@export var direction:Vector3
@export var dopplerFactor:float
@export var rolloff:float
@export var coneInnerAngle:float
@export var coneOuterAngle:float
@export var coneOuterGain:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"minDistance" : FormatWriter.format(minDistance, "minDistance"),
		"maxDistance" : FormatWriter.format(maxDistance, "maxDistance"),
		"velocity" : FormatWriter.format(velocity, "velocity"),
		"direction" : FormatWriter.format(direction, "direction"),
		"dopplerFactor" : FormatWriter.format(dopplerFactor, "dopplerFactor"),
		"rolloff" : FormatWriter.format(rolloff, "rolloff"),
		"coneInnerAngle" : FormatWriter.format(coneInnerAngle, "coneInnerAngle"),
		"coneOuterAngle" : FormatWriter.format(coneOuterAngle, "coneOuterAngle"),
		"coneOuterGain" : FormatWriter.format(coneOuterGain, "coneOuterGain"),
	}
