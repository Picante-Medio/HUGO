@tool
extends Entity
class_name Entity3D

var last_pos:Vector3
var last_rot:Vector3
var last_scale:Vector3

func _ready() -> void:
	super()
	update_transform()

func _process(_delta: float) -> void:
	super(_delta)
	if global_position == last_pos: return
	update_transform()

func update_transform():
	var scene_tracker:SCENE_TRACKER = get_tree().get_first_node_in_group(&"SCENE_TRACKER")
	var m_transform:Dictionary = scene_tracker.entities[name].get_or_add("TRANSFORM", {})
	
	m_transform.get_or_add("global_position", global_position)
	m_transform["global_position"] = global_position
	last_pos = global_position
	
	m_transform.get_or_add("scale", scale)
	m_transform["scale"] = scale
	last_scale = scale
	
	m_transform.get_or_add("rotation", rotation)
	m_transform["rotation"] = rotation
	last_rot = rotation
