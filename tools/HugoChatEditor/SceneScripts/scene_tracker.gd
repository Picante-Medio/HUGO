@icon("res://Icons/scene_tracker_icon.png")
@tool
extends Node3D
class_name SCENE_TRACKER

@export var entities:Dictionary[String, Dictionary]
@export var systems:Array[SYSTEMS]
@export var prefabs:Array[StringName]

@export_tool_button("Save Scene") var button = SceneWriter.write

var current_entity_index:int = 0
var entities_map:Dictionary[String, int]

func create_entities():
	entities_map = {}
	entities = {}
	current_entity_index = 0
	
	if self is Entity: add_entity(self)
	gather_entities(self)

func add_entity(entity:Entity):
	var entity_name:String = entity.name
	while entities.has(entity_name):
		entity_name = entity_name + str("_O")
	
	if self is not Entity and entity.scene_file_path != "":
		print(scene_file_path + " is not " + entity.scene_file_path)
		var scene_paths:PackedStringArray = entity.scene_file_path.split("/")
		var scene_name:String = scene_paths[scene_paths.size()-1].split(".tscn")[0]
		if not prefabs.has(scene_name): prefabs.push_back(scene_name)
		
	entity.name = entity_name
	entities.get_or_add(entity_name, {})
	entities_map.get_or_add(entity_name, current_entity_index)
	current_entity_index += 1

func gather_entities(parent:Node):
	for child in parent.get_children():
		
		if child is Entity:
			add_entity(child)
			
			if parent is Entity:
				entities.get(child.name).get_or_add("Parent", {"value int" : entities_map[parent.name]})
				
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
