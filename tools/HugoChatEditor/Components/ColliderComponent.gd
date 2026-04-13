@tool
extends Component
class_name ColliderComponent

@export var restitution:float
@export var friction:float
@export var positionOffset:Vector3
@export var rotationOffset:Quaternion
@export var active:bool
@export var useGizmo:bool
@export var info:ENUMS.ColliderType

func get_component() -> Dictionary[String, Variant]:
	return {
		"restitution" : FormatWriter.format(restitution, "restitution"),
		"friction" : FormatWriter.format(friction, "friction"),
		"positionOffset" : FormatWriter.format(positionOffset, "positionOffset"),
		"rotationOffset" : FormatWriter.format(rotationOffset, "rotationOffset"),
		"active" : FormatWriter.format(active, "active"),
		"useGizmo" : FormatWriter.format(useGizmo, "useGizmo"),
		"info" : FormatWriter.format(info, "info"),
	}
