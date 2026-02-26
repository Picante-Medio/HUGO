extends Component
class_name AudioComponent

@export var type:ENUMS.AudioTypes
@export var soundId:int
@export var volume:float
@export var pitch:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"type" : type,
		"soundId" : soundId,
		"volume" : volume,
		"pitch" : pitch,
	}
