@tool
extends Component
class_name AudioComponent

@export var type:int
@export var audioName:String
@export var volume:float
@export var loop:bool
@export var pitch:float
@export var startTime:float
@export var isPlaying:bool

func get_component() -> Dictionary[String, Variant]:
	return {
		"type" : FormatWriter.format(type, "type"),
		"audioName" : FormatWriter.format(audioName, "audioName"),
		"volume" : FormatWriter.format(volume, "volume"),
		"loop" : FormatWriter.format(loop, "loop"),
		"pitch" : FormatWriter.format(pitch, "pitch"),
		"startTime" : FormatWriter.format(startTime, "startTime"),
		"isPlaying" : FormatWriter.format(isPlaying, "isPlaying"),
	}
