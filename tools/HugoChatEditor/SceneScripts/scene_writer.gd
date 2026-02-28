@tool
extends EditorScript
class_name SceneWriter

const CHATS_PATH = "../../HUGO/chats/"

func _run() -> void:
	write()

static func write():
	var root:SCENE_TRACKER = EditorInterface.get_edited_scene_root()
	if root == null:
		push_error("No edited scene root found.")
		return
	
	root.create_entities()
	var entity_map:Dictionary[String, int] = root.entities_map
	
	var scene_name := root.name
	var file_path := CHATS_PATH.path_join("%s.chat" % scene_name)
	
	var file := FileAccess.open(file_path, FileAccess.WRITE)
	if file == null:
		push_error("Could not open file for writing: %s" % file_path)
		return
	
	for prefab:StringName in root.prefabs:
		file.store_line("PREFAB " + prefab + ".chat")
	file.store_line("")
	
	if root is not Entity:
		for system:SCENE_TRACKER.SYSTEMS in root.systems:
			var system_name:String = SCENE_TRACKER.SYSTEMS.keys()[system]
			file.store_line("SYSTEM " + system_name)
		file.store_line("")
	
	for entity_key in root.entities.keys():
		file.store_line("ENTITY " + str(entity_key) + " " + str(entity_map[entity_key]))
		
		var entity: Dictionary = root.entities[entity_key]
		
		for component_key in entity.keys():
			file.store_line("\tCOMPONENT " + str(component_key))
			
			var component: Dictionary = entity[component_key]
			
			for parameter_key in component.keys():
				var value = component[parameter_key]
				file.store_line("\t\t[%s] %s" % [parameter_key, str(value)])
		
		file.store_line("")
	
	file.close()
	print("Written to: ", file_path)
