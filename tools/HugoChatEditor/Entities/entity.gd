@tool
@icon("res://Icons/entity_icon.png")
extends Node3D
class_name Entity

var last_parent:String
var last_name:String

func _ready() -> void:
	print("Added " + name + " entity")
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	if scene_tracker.entities.has(name):
		name = VERY_IMPORTANT_DATA_BASE.get_random_profile()
	scene_tracker.entities.get_or_add(name, {})
	
	last_name = name
	renamed.connect(update_name)
	update_parent()

func update_name():
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	print(last_name)
	print(name)
	var data:Dictionary = scene_tracker.entities[last_name]
	scene_tracker.entities.get_or_add(name, data)
	scene_tracker.entities[name] = data
	scene_tracker.entities.erase(last_name)
	last_name = name

func _process(_delta: float) -> void:
	if last_parent != get_parent().name:
		update_parent()

func update_parent():
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var entity:Dictionary = scene_tracker.entities.get_or_add(name, {})
	
	if get_parent() is Entity:
		print("I HAVE A FATHER")
		scene_tracker.entities.get_or_add(name)
		var parent_component:Dictionary = entity.get_or_add("PARENT", {})
		parent_component.get_or_add("value", get_parent().name)
		parent_component["value"] = get_parent().name
	else:
		entity.erase("PARENT")
		
	last_parent = get_parent().name

func _exit_tree() -> void:
	print("CHAO CHAO NODO")
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	scene_tracker.entities.erase(name)
