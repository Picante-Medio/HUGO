@tool
extends EditorPlugin

var plugin
var config_dock

func _enable_plugin() -> void:
	scene_saved.connect(save_scene)

func _disable_plugin() -> void:
	if scene_saved.is_connected(save_scene): scene_saved.disconnect(save_scene)

func _enter_tree():
	plugin = preload("res://addons/HugosChatPlugin/hugos_chat_plugin.gd").new()
	add_inspector_plugin(plugin)
	
	config_dock = preload("res://addons/HugosChatPlugin/Dock/hugo_config.tscn").instantiate()
	add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UR, config_dock)

func save_scene(_scene_path):
	if Input.is_physical_key_pressed(KEY_R):
		ScriptsSyncronizer.sync()
		print("SCRIPTS SYNC...")
	else: SceneWriter.write()

func _exit_tree():
	remove_inspector_plugin(plugin)
	remove_control_from_docks(config_dock)
	config_dock.free()
