extends Component
class_name Sound3DInfoComponent

@export var minDistance:float
@export var maxDistance:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"minDistance" : minDistance,
		"maxDistance" : maxDistance,
	}
