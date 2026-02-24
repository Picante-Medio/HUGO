@icon("res://Icons/component_free_style.png")
@tool
extends Component
class_name JumpHandler

@export var jump_force:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"jump_force" : jump_force
	}
