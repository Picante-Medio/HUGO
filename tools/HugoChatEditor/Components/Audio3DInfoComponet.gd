@tool
extends Component
class_name Audio3DInfoComponet

@export var audioId:int
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
		"audioId" : FormatWriter.format(audioId),
		"minDistance" : FormatWriter.format(minDistance),
		"maxDistance" : FormatWriter.format(maxDistance),
		"velocity" : FormatWriter.format(velocity),
		"direction" : FormatWriter.format(direction),
		"dopplerFactor" : FormatWriter.format(dopplerFactor),
		"rolloff" : FormatWriter.format(rolloff),
		"coneInnerAngle" : FormatWriter.format(coneInnerAngle),
		"coneOuterAngle" : FormatWriter.format(coneOuterAngle),
		"coneOuterGain" : FormatWriter.format(coneOuterGain),
	}
