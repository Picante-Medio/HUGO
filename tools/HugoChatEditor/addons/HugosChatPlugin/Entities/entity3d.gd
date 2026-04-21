@tool
extends Entity
class_name Entity3D

func get_component() -> Dictionary[String, Variant]:
	return {
		"position" : FormatWriter.format(position, "position"),
		"rotation" : FormatWriter.format(global_basis.get_rotation_quaternion(), "rotation"),
		"scale" : FormatWriter.format(scale, "scale")
	}
