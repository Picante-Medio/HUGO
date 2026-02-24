@icon("res://Icons/scene_tracker_icon.png")
@tool
extends Node
class_name SCENE_TRACKER

@export var entities:Dictionary[String, Dictionary]
@export var systems:Array[SYSTEMS]

var current_entity_index:int = 0
var entities_map:Dictionary[String, int]

func create_entities():
	entities_map = {}
	entities = {}
	current_entity_index = 0
	gather_entities(self)

func add_entity(entity:Entity):
	var entity_name:String = entity.name
	while entities.has(entity_name):
		entity_name = entity_name + str("_O")
	entity.name = entity_name
	entities.get_or_add(entity_name, {})
	entities_map.get_or_add(entity_name, current_entity_index)
	current_entity_index += 1

func gather_entities(parent:Node):
	for child in parent.get_children():
		
		if child is Entity:
			add_entity(child)
			
			if parent is Entity:
				entities.get(child.name).get_or_add("Parent", {"value" : entities_map[parent.name]})
				
			gather_entities(child)
			
			if child is Entity3D:
				entities.get(child.name).get_or_add("Transform", child.get_component())
			
		if child.has_method("get_component"):
			if parent is Entity:
				entities.get(parent.name).get_or_add(child.get_class(), child.get_component())
				
	return entities

enum SYSTEMS {
	MOVEMENT_SYSTEM,
	CHAT_SYSTEM,
	ZAPAS_SYSTEM,
	SKIBIDI_SYSTEM,
	HUGO_SYSTEM
}
