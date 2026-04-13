@tool
extends Component
class_name AudioListenerComponent

@export var listenerVelocity:Vector3
@export var masterVolume:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"listenerVelocity" : FormatWriter.format(listenerVelocity, "listenerVelocity"),
		"masterVolume" : FormatWriter.format(masterVolume, "masterVolume"),
	}
