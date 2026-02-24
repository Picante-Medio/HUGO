@icon("res://Icons/component_free_style.png")
@tool
extends Node
class_name JumpHandler

func _ready() -> void:
	update_jump_force()

@export var jump_force:float:
	set(new_force):
		jump_force = new_force
		update_jump_force()
		
func update_jump_force():
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(get_parent().name, {})
	var jump_handler:Dictionary = entity.get_or_add("JUMP_HANDLER", {})
	
	jump_handler.get_or_add("jump_force", jump_force)
	jump_handler["jump_force"] = jump_force

func _exit_tree() -> void:
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(get_parent().name, {})
	entity.erase("JUMP_HANDLER")
