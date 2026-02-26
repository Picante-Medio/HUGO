extends Component
class_name AudioComponent

@export var type:ENUMS.AudioTypes
@export var audioId:int
@export var volume:float
@export var pitch:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"type" : type,
		"audioId" : audioId,
		"volume" : volume,
		"pitch" : pitch,
	}
