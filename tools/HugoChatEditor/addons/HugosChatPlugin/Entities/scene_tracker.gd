@icon("res://addons/HugosChatPlugin/Icons/scene_tracker_icon.png")
@tool
extends Node3D
class_name SCENE_TRACKER

@export var entities:Dictionary[String, Dictionary]
@export var systems:Array[SYSTEMS.system] = [
	SYSTEMS.system.ConsoleSystem,
	SYSTEMS.system.AudioSystem,
	SYSTEMS.system.PhysicsGizmoSystem,
	SYSTEMS.system.HierarchySystem,
	SYSTEMS.system.ParticleSystem,
	SYSTEMS.system.CameraSystem,
	SYSTEMS.system.LightSystem,
	SYSTEMS.system.RenderSystem,
	SYSTEMS.system.TweenSystem,
	SYSTEMS.system.UISystem
]

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
		
	entity.name = entity_name
	entities.get_or_add(entity_name, {})
	entities_map.get_or_add(entity_name, current_entity_index)
	current_entity_index += 1

func gather_entities(parent:Node):
	for child in parent.get_children():
		
		if child is Entity:
			add_entity(child)
			
			if parent is Entity:
				entities.get(child.name).get_or_add("ParentComponent", {"value" :  "INT " + str(entities_map[parent.name])})
				
			gather_entities(child)
			
			if child is Entity3D:
				entities.get(child.name).get_or_add("TransformComponent", child.get_component())
			
		if child.has_method("get_component"):
			if parent is Entity:
				var child_class_name:String = child.get_script().resource_path.get_file().split(".gd")[0]
				entities.get(parent.name).get_or_add(child_class_name, child.get_component())
				
	return entities
