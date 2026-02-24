@tool
extends Entity
class_name Entity3D

func get_component() -> Dictionary[String, Variant]:
	return {
		"position" : position,
		"rotation" : rotation,
		"scale" : scale
	}
