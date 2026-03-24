@tool
extends Entity
class_name Entity3D

func get_component() -> Dictionary[String, Variant]:
	return {
		"position" : FormatWriter.format(position, "position"),
		"rotation" : FormatWriter.format(rotation, "rotation"),
		"scale" : FormatWriter.format(scale, "scale")
	}
