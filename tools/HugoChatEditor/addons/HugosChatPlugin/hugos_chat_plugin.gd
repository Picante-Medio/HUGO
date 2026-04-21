@tool
extends EditorPlugin

var plugin

func _enable_plugin() -> void:
	scene_saved.connect(save_scene)

func _disable_plugin() -> void:
	if scene_saved.is_connected(save_scene): scene_saved.disconnect(save_scene)

func _enter_tree():
	plugin = preload("res://addons/HugosChatPlugin/hugos_chat_plugin.gd").new()
	add_inspector_plugin(plugin)

func save_scene(_scene_path):
	if Input.is_physical_key_pressed(KEY_R):
		ScriptsSyncronizer.sync()
		print("SCRIPTS SYNC...")
	else: SceneWriter.write()

func _exit_tree():
	remove_inspector_plugin(plugin)
