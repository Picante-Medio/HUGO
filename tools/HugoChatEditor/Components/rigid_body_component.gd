@icon("res://Icons/component_free_style.png")
@tool
extends RigidBody3D
class_name RigidbodyComponent

func _exit_tree() -> void:
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(get_parent().name, {})
	entity.erase("RIGIDBODY")
