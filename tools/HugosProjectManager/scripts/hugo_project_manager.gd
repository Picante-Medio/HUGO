extends Control
class_name HugoProjectManager

const ADDONS_ZIP_PATH := "res://addons.zip"
const REQUIRED_FOLDERS := ["Resources", "Components", "Systems"]
const ENGINE_PATH := ""

@onready var project_name_line_edit: LineEdit = $PanelContainer/MarginContainer/VBoxContainer/ProjectNameLineEdit
@onready var location_line_edit: LineEdit = $PanelContainer/MarginContainer/VBoxContainer/LocationRow/LocationLineEdit
@onready var browse_button: Button = $PanelContainer/MarginContainer/VBoxContainer/LocationRow/BrowseButton
@onready var create_button: Button = $PanelContainer/MarginContainer/VBoxContainer/CreateButton
@onready var status_label: RichTextLabel = $PanelContainer/MarginContainer/VBoxContainer/StatusLabel
@onready var file_dialog: FileDialog = $FileDialog

@export var world:Node2D
var project_scene = preload("res://scenes/project_entity.tscn")
var projects_paths:Array[String]

func _ready() -> void:
	load_save()
	
	browse_button.pressed.connect(_on_browse_pressed)
	create_button.pressed.connect(_on_create_pressed)
	file_dialog.dir_selected.connect(_on_dir_selected)

	location_line_edit.text = OS.get_system_dir(OS.SYSTEM_DIR_DOCUMENTS)
	_print_status("Listo. Configura nombre y ruta para crear un proyecto.")

func _on_browse_pressed() -> void:
	file_dialog.current_dir = location_line_edit.text
	file_dialog.popup_centered_ratio(0.7)

func _on_dir_selected(dir_path: String) -> void:
	location_line_edit.text = dir_path

func _on_create_pressed() -> void:
	var project_name := project_name_line_edit.text.strip_edges()
	var destination_root := location_line_edit.text.strip_edges()

	if project_name.is_empty():
		_print_error("Debes indicar un nombre de proyecto.")
		return

	if destination_root.is_empty() or not DirAccess.dir_exists_absolute(destination_root):
		_print_error("La ruta destino no existe o no es valida.")
		return

	var project_dir := destination_root.path_join(project_name)
	if DirAccess.dir_exists_absolute(project_dir):
		_print_error("Ya existe una carpeta con ese nombre: %s" % project_dir)
		return

	create_button.disabled = true
	var result := _create_project(project_name, project_dir)
	create_button.disabled = false

	if result:
		_print_success("Proyecto creado en: %s" % project_dir)
		add_project(project_dir)
	else:
		_print_error("No se pudo crear el proyecto. Revisa los mensajes anteriores.")

func _create_project(project_name: String, project_dir: String) -> bool:
	_print_status("Creando carpeta del proyecto...")
	var mkdir_result := DirAccess.make_dir_recursive_absolute(project_dir)
	if mkdir_result != OK:
		_print_error("No se pudo crear la carpeta de proyecto (codigo %d)." % mkdir_result)
		return false

	for folder_name in REQUIRED_FOLDERS:
		var folder_path := project_dir.path_join(folder_name)
		var folder_result := DirAccess.make_dir_recursive_absolute(folder_path)
		if folder_result != OK:
			_print_error("No se pudo crear la carpeta %s (codigo %d)." % [folder_name, folder_result])
			return false

	_print_status("Escribiendo project.godot...")
	if not _write_project_file(project_name, project_dir):
		return false

	_print_status("Descomprimiendo addons.zip en la raiz...")
	if not _extract_addons_zip(project_dir):
		return false

	return true

func _write_project_file(project_name: String, file_dir: String) -> bool:
	var project_godot := file_dir.path_join("project.godot")
	var file := FileAccess.open(project_godot, FileAccess.WRITE)
	if file == null:
		_print_error("No se pudo crear project.godot en %s" % project_godot)
		return false

	file.store_string(_build_project_config(project_name))
	file.close()
	
	var launch_config := file_dir.path_join("launch.config")
	var config_file := FileAccess.open(launch_config, FileAccess.WRITE)
	if config_file == null:
		_print_error("No se pudo crear launch.config en %s" % launch_config)
		return false
		
	var folder = OS.get_executable_path().get_base_dir()
	config_file.store_line(folder.path_join("/bin/HUGO.exe"))
	config_file.store_line(file_dir)
	config_file.store_line(file_dir.path_join("Chats"))
	config_file.store_line("main_scene.tscn")
	
	config_file.close()
	
	compile_executable(file_dir)
	return true

func compile_executable(output_dir_path:String) -> int:
	var absolute_file_path = ProjectSettings.globalize_path("res://launcher.c")
	var salida = []

	var codigo = OS.execute("gcc", [absolute_file_path, "-o", output_dir_path.path_join("play.exe")], salida, true)

	print("Salida:", salida)
	return codigo

func _extract_addons_zip(project_dir: String) -> bool:
	var zip := ZIPReader.new()
	var open_result := zip.open(ADDONS_ZIP_PATH)
	if open_result != OK:
		_print_error("No se pudo abrir %s (codigo %d)." % [ADDONS_ZIP_PATH, open_result])
		return false

	for zip_path in zip.get_files():
		var output_path := project_dir.path_join(zip_path)

		if zip_path.ends_with("/"):
			var dir_result := DirAccess.make_dir_recursive_absolute(output_path)
			if dir_result != OK:
				zip.close()
				_print_error("No se pudo crear carpeta del zip: %s" % zip_path)
				return false
			continue

		var parent_dir := output_path.get_base_dir()
		var parent_result := DirAccess.make_dir_recursive_absolute(parent_dir)
		if parent_result != OK:
			zip.close()
			_print_error("No se pudo crear carpeta padre: %s" % parent_dir)
			return false

		var content := zip.read_file(zip_path)
		var output_file := FileAccess.open(output_path, FileAccess.WRITE)
		if output_file == null:
			zip.close()
			_print_error("No se pudo escribir archivo: %s" % output_path)
			return false

		output_file.store_buffer(content)
		output_file.close()

	zip.close()
	return true

func _build_project_config(project_name: String) -> String:
	return """; Engine configuration file.
; It's best edited using the editor UI and not directly,
; since the parameters that go here are not all obvious.
;
; Format:
;   [section] ; section goes between []
;   param=value ; assign values to parameters

config_version=5

[application]

config/name=\"%s\"
config/features=PackedStringArray(\"4.6\", \"GL Compatibility\")
config/icon=\"res://icon.svg\"
config/hugo/exe_path=\"res://game.exe\"

[editor_plugins]

enabled=PackedStringArray(\"res://addons/HugosChatPlugin/plugin.cfg\")

[rendering]

rendering_device/driver.windows=\"d3d12\"
renderer/rendering_method=\"gl_compatibility\"
renderer/rendering_method.mobile=\"gl_compatibility\"
""" % project_name

func _print_status(message: String) -> void:
	status_label.append_text("[color=#d1d5db]%s[/color]\n" % message)

func _print_success(message: String) -> void:
	status_label.append_text("[color=#8be28b]%s[/color]\n" % message)

func _print_error(message: String) -> void:
	status_label.append_text("[color=#ff7b7b]%s[/color]\n" % message)

func add_project(path:String):
	projects_paths.push_back(path)
	instance_project(path)
	save()

func remove_project(path:String):
	projects_paths.erase(path)
	save()

func load_save():
	var save_file = FileAccess.open("user://savegame.save", FileAccess.READ)
	if not save_file: return
	
	var current_line:String = save_file.get_line().strip_edges()
	while not save_file.eof_reached():
		if DirAccess.dir_exists_absolute(current_line):
			projects_paths.push_back(current_line)
		current_line = save_file.get_line().strip_edges()
	save_file.close()
	
	for project_path in projects_paths:
		instance_project(project_path)

func instance_project(path:String):
	var project:Draggable = project_scene.instantiate()
	world.add_child(project)
	project.set_path(path)

func save():
	var save_file = FileAccess.open("user://savegame.save", FileAccess.WRITE)
	for path in projects_paths:
		save_file.store_line(path)
	save_file.close()
