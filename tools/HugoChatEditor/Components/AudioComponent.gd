@tool
extends Component
class_name AudioComponent

@export var audioId:int
@export var volume:float
@export var pitch:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"audioId" : FormatWriter.format(audioId),
		"volume" : FormatWriter.format(volume),
		"pitch" : FormatWriter.format(pitch),
	}
