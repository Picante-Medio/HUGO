@tool
extends EditorPlugin

var play_button
var plugin
var config_dock

func _enable_plugin() -> void:
	scene_saved.connect(save_scene)

func _disable_plugin() -> void:
	if scene_saved.is_connected(save_scene): scene_saved.disconnect(save_scene)

func _enter_tree():
	
	plugin = preload("res://addons/HugosChatPlugin/hugos_chat_plugin.gd").new()
	add_inspector_plugin(plugin)
	
	#play_button = PlayButton.new()
	#play_button.text = "Run"
	#play_button.icon = get_editor_interface().get_base_control().get_theme_icon("Play", "EditorIcons")
	#play_button.custom_minimum_size = Vector2(80, 28)
	#play_button.flat = false
	#add_control_to_container(CONTAINER_TOOLBAR, play_button)
	
	config_dock = preload("res://addons/HugosChatPlugin/Dock/hugo_config.tscn").instantiate()
	add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, config_dock)

func save_scene(_scene_path):
	if Input.is_physical_key_pressed(KEY_R):
		ScriptsSyncronizer.sync()
		print("SCRIPTS SYNC...")
	else: SceneWriter.write()

func _exit_tree():
	remove_inspector_plugin(plugin)
	
	remove_control_from_docks(config_dock)
	remove_control_from_container(CONTAINER_TOOLBAR, play_button)
	
	config_dock.free()
	play_button.free()
