@tool
extends EditorScript
class_name ScriptsSyncronizer

const COMPONENTS_PATH = "../../HUGO/src/components"
const OUTPUT_COMPONENTS_PATH = "Components"
const OUTPUT_CHAT_PARSER_PATH = "../../HUGO/src/ecs/ChatParser.cpp"
const HUGO_TYPES_PATH = "../../HUGO/src/utils/HugoTypes.h"

const CHATS_PATH = "../../HUGO/chats/"
const PREFABS_PATH = "res://Prefabs/"

const OUTPUT_ENUMS_PATH = "Enums.gd"

var enums_dictionary:Dictionary
var types_dictionary:Dictionary
var components_dictionary:Dictionary

const WHITE_LIST:Array[String] = ["string", "int", "float", "Vector2", "Vector3", "Vector4", "Quaternion"]
const BLACK_LIST:Array[String] = ["TransformComponent", "CameraComponent", "RenderComponent"]

func _run() -> void:
	parse_hugo_types()
	write_enums()
	write_components()
	parse_components(COMPONENTS_PATH)
	create_prefabs(PREFABS_PATH)
	EditorInterface.get_resource_filesystem().scan_sources()

func create_prefabs(path:String):
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

func ensure_folder(path: String) -> void:
	var abs_path := ProjectSettings.globalize_path(path)
	if DirAccess.dir_exists_absolute(abs_path): return

	var err := DirAccess.make_dir_recursive_absolute(abs_path)
	if err != OK:
		push_error("Error creando carpeta: " + abs_path)

func write_components():
	ensure_folder(OUTPUT_COMPONENTS_PATH)

	for key in components_dictionary.keys():
		var value = components_dictionary[key]
		print("\nCOMPONENT : " + key)
		for v in value:
			print(v[0] + " " + v[1])

	for component_name:String in components_dictionary.keys():
		var file := FileAccess.open(OUTPUT_COMPONENTS_PATH.path_join(component_name + ".gd"), FileAccess.WRITE)

		file.store_line("@tool")
		file.store_line("extends Component")
		file.store_line("class_name " + component_name)
		file.store_line("")

		var component_parameters = components_dictionary[component_name]
		for parameter:Array in component_parameters:
			var new_parameter:String = "@export var " + parameter[1] + ":" + parameter[0]
			file.store_line(new_parameter)

		file.store_line("")
		file.store_line("func get_component() -> Dictionary[String, Variant]:")
		file.store_line("\treturn {")

		for parameter:Array in component_parameters:
			var new_parameter:String = '\t\t"' + parameter[1] + '"' + " : FormatWriter.format(" + parameter[1] + "),"
			file.store_line(new_parameter)

		file.store_line("\t}")

		file.close()

func write_enums():
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

func hugo_check_line(line: String) -> void:
	if line.begins_with("using "):
		var alias_name:String = line.split("using")[1].split("=")[0].strip_edges()
		var variant_name:String = line.split("=")[1].split(";")[0].strip_edges()
		types_dictionary.get_or_add(alias_name, variant_name)

	if line.begins_with("typedef enum"):
		var enum_values:PackedStringArray = line.split("{")[1].split("}")[0].split(",")
		var enum_name:String = line.split("}")[1].split(";")[0].strip_edges()
		enums_dictionary.get_or_add(enum_name, enum_values)

func parse_hugo_types() -> void:
	var file := FileAccess.open(HUGO_TYPES_PATH, FileAccess.READ)

	while not file.eof_reached():
		var line := file.get_line().strip_edges()
		hugo_check_line(line)

	file.close()

func parse_components(path: String) -> void:
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

func hugo_parse_component(path: String) -> void:
	var file := FileAccess.open(path, FileAccess.READ)
	var line := file.get_line().strip_edges()

	while not line.contains(":") and not file.eof_reached():
		line = file.get_line().strip_edges()

	if line.split(":")[1].strip_edges() != "public Component": return
	var component_name := line.split("struct")[1].split(":")[0].strip_edges()
	if BLACK_LIST.has(component_name): return

	components_dictionary.get_or_add(component_name, [])

	var complete_whitelist:Array[String]
	complete_whitelist.append_array(enums_dictionary.keys())
	complete_whitelist.append_array(WHITE_LIST)
	complete_whitelist.append_array(types_dictionary.keys())

	while not file.eof_reached():
		line = file.get_line().strip_edges()
		if not line.contains(";"): continue

		var cleaned_parameter:String = line.split("::")[1] if line.split("::").size() > 1 else line.split("::")[0]
		cleaned_parameter = cleaned_parameter.split("=")[0]
		cleaned_parameter = cleaned_parameter.split(";")[0]
		cleaned_parameter = cleaned_parameter.strip_edges()

		var parameter:PackedStringArray = cleaned_parameter.split(" ")
		if parameter.size() != 2: continue
		var parameter_type := parameter[0].strip_edges()
		var parameter_name := parameter[1].strip_edges()
		if parameter_name.contains("(") or not complete_whitelist.has(parameter_type): continue

		if parameter_type == "string": parameter_type = "String"
		elif types_dictionary.has(parameter_type): parameter_type = types_dictionary[parameter_type]
		elif enums_dictionary.has(parameter_type): parameter_type = "ENUMS." + parameter_type

		components_dictionary[component_name].push_back([parameter_type, parameter_name])

	file.close()
