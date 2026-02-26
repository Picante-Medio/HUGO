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
		"audioId" : audioId,
		"minDistance" : minDistance,
		"maxDistance" : maxDistance,
		"velocity" : velocity,
		"direction" : direction,
		"dopplerFactor" : dopplerFactor,
		"rolloff" : rolloff,
		"coneInnerAngle" : coneInnerAngle,
		"coneOuterAngle" : coneOuterAngle,
		"coneOuterGain" : coneOuterGain,
	}
