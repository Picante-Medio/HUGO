@tool
extends EditorScript

func generate_entity_map(entities:Dictionary[String, Dictionary]) -> Dictionary[String, int]:
	var entity_map: Dictionary[String, int]
	var i:int = 0
	for entity_key in entities.keys():
		entity_map.get_or_add(entity_key, i)
		i += 1
	return entity_map

func _run() -> void:
	var root: SCENE_TRACKER = EditorInterface.get_edited_scene_root()
	if root == null:
		push_error("No edited scene root found.")
		return
	
	var entity_map:Dictionary[String, int] = generate_entity_map(root.entities)
	
	var scene_name := root.name
	var file_path := "res://%s.chat" % scene_name
	
	var file := FileAccess.open(file_path, FileAccess.WRITE)
	if file == null:
		push_error("Could not open file for writing: %s" % file_path)
		return
	
	for system:SCENE_TRACKER.SYSTEMS in root.systems:
		var system_name:String = SCENE_TRACKER.SYSTEMS.keys()[system]
		file.store_line("SYSTEM : " + system_name)
	file.store_line("")
	
	for entity_key in root.entities.keys():
		file.store_line("ENTITY : " + str(entity_key) + " | " + str(entity_map[entity_key]))
		
		var entity: Dictionary = root.entities[entity_key]
		
		for component_key in entity.keys():
			file.store_line("\tCOMPONENT : " + str(component_key))
			
			var component: Dictionary = entity[component_key]
			
			for parameter_key in component.keys():
				var value = component[parameter_key]
				if component_key == "PARENT":
					file.store_line("\t\t%s : %s" % [parameter_key, str(entity_map[value])])
				else:
					file.store_line("\t\t%s : %s" % [parameter_key, str(value)])
		
		file.store_line("")
	
	file.close()
	print("Written to: ", file_path)
