@tool
extends EditorScript
class_name ScriptsSyncronizer

const components_path = "../../HUGO/include/components"
const output_components_path = "Components"
const HUGO_TYPES_PATH = "../../HUGO/include/utils/HugoTypes.h"

const CHATS_PATH = "../../HUGO/chats/"
const PREFABS_PATH = "res://Prefabs/"

const OUTPUT_ENUMS_PATH = "Enums.gd"

static var enums_dictionary:Dictionary
static var types_dictionary:Dictionary
static var components_dictionary:Dictionary

const WHITE_LIST:Array[String] = ["string", "int", "float", "Vector2", "Vector3", "Vector4", "Quaternion"]
const BLACK_LIST:Array[String] = ["TransformComponent", "CameraComponent", "RenderComponent"]

func _run() -> void:
	sync()
	
static func sync():
	components_dictionary = {}
	enums_dictionary = {}
	components_dictionary = {}
	
	parse_hugo_types()
	write_enums()
	parse_components(components_path)
	create_prefabs(PREFABS_PATH)
	write_components()
	EditorInterface.get_resource_filesystem().scan_sources()

static func create_prefabs(path:String):
	var dir := DirAccess.open(path)

	dir.list_dir_begin()

	var name := dir.get_next()

	while name != "":
		if name != "." and name != "..":
			var full_path := PREFABS_PATH.path_join(name)

			if dir.current_is_dir():
				create_prefabs(full_path)
			else:
				EditorInterface.open_scene_from_path(full_path)
				SceneWriter.write()
				EditorInterface.close_scene()

		name = dir.get_next()

	dir.list_dir_end()

static func ensure_folder(path: String) -> void:
	var abs_path := ProjectSettings.globalize_path(path)
	if DirAccess.dir_exists_absolute(abs_path): return

	var err := DirAccess.make_dir_recursive_absolute(abs_path)
	if err != OK:
		push_error("Error creando carpeta: " + abs_path)

static func write_components():
	ensure_folder(output_components_path)

	for component_name:String in components_dictionary.keys():
		var file := FileAccess.open(output_components_path.path_join(component_name + ".gd"), FileAccess.WRITE)

		file.store_line("@tool")
		file.store_line("extends Component")
		file.store_line("class_name " + component_name)
		file.store_line("")

		var component_parameters = components_dictionary[component_name]
		for parameter:Array in component_parameters:
			var value_text:String = ""
			if parameter[2]: value_text = " = " + parameter[2]
			var new_parameter:String = "@export var " + parameter[1] + ":" + parameter[0] + value_text
			file.store_line(new_parameter)

		file.store_line("")
		file.store_line("func get_component() -> Dictionary[String, Variant]:")
		file.store_line("\treturn {")

		for parameter:Array in component_parameters:
			var new_parameter:String = '\t\t"' + parameter[1] + '"' + " : FormatWriter.format(" + parameter[1] + ', "' + parameter[1] + '"),'
			file.store_line(new_parameter)

		file.store_line("\t}")

		file.close()

static func write_enums():
	var file := FileAccess.open(OUTPUT_ENUMS_PATH, FileAccess.WRITE)
	file.store_line("class_name ENUMS")

	for enum_key in enums_dictionary.keys():
		var enum_value = enums_dictionary[enum_key]
		var text_to_write:String = "enum " + enum_key + "{"
		for value in enum_value:
			text_to_write = text_to_write + value + ","
		text_to_write = text_to_write + "}"
		file.store_line(text_to_write)

	file.close()

static func hugo_check_line(line: String) -> void:
	if line.begins_with("using "):
		var alias_name:String = line.split("using")[1].split("=")[0].strip_edges()
		var variant_name:String = line.split("=")[1].split(";")[0].strip_edges()
		types_dictionary.get_or_add(alias_name, variant_name)

	if line.begins_with("typedef enum"):
		var enum_values:PackedStringArray = line.split("{")[1].split("}")[0].split(",")
		var enum_name:String = line.split("}")[1].split(";")[0].strip_edges()
		enums_dictionary.get_or_add(enum_name, enum_values)

static func parse_hugo_types() -> void:
	var file := FileAccess.open(HUGO_TYPES_PATH, FileAccess.READ)

	while not file.eof_reached():
		var line := file.get_line().strip_edges()
		hugo_check_line(line)

	file.close()

static func parse_components(path: String) -> void:
	var dir := DirAccess.open(path)

	dir.list_dir_begin()

	var name := dir.get_next()
	
	while name != "":
		if name != "." and name != "..":
			var full_path := path.path_join(name)

			if dir.current_is_dir():
				parse_components(full_path)
			else:
				hugo_parse_component(full_path)

		name = dir.get_next()

	dir.list_dir_end()

static func hugo_parse_component(path: String) -> void:
	var file := FileAccess.open(path, FileAccess.READ)
	var line := file.get_line().strip_edges()

	while not line.contains("HCOMPONENT(") and not file.eof_reached():
		line = file.get_line().strip_edges()

	var component_name:String = line.split("(")[1].split(")")[0].strip_edges()
	if BLACK_LIST.has(component_name): return

	components_dictionary.get_or_add(component_name, [])

	var complete_whitelist:Array[String]
	complete_whitelist.append_array(enums_dictionary.keys())
	complete_whitelist.append_array(WHITE_LIST)
	complete_whitelist.append_array(types_dictionary.keys())

	print(path)
	while not file.eof_reached():
		line = file.get_line().strip_edges()
		
		if not line.contains("@export"): continue
		else: line = file.get_line().strip_edges()

		var cleaned_parameter:String = line.split("::")[1] if line.split("::").size() > 1 else line.split("::")[0]
		cleaned_parameter = cleaned_parameter.split(";")[0]
		cleaned_parameter = cleaned_parameter.split("=")[0]
		
		cleaned_parameter = cleaned_parameter.strip_edges()
		var parameter_default_value
		if cleaned_parameter[1]: parameter_default_value = cleaned_parameter[1]

		var parameter:PackedStringArray = cleaned_parameter.split(" ")
		if parameter.size() != 2: continue
		var parameter_type := parameter[0].strip_edges()
		var parameter_name := parameter[1].strip_edges()
		if parameter_name.contains("(") or not complete_whitelist.has(parameter_type): continue

		if parameter_type == "string": parameter_type = "String"
		elif types_dictionary.has(parameter_type): parameter_type = types_dictionary[parameter_type]
		elif enums_dictionary.has(parameter_type): parameter_type = "ENUMS." + parameter_type

		components_dictionary[component_name].push_back([parameter_type, parameter_name, parameter_default_value])

	file.close()
