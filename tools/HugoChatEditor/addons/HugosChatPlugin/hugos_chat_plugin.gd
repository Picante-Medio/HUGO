@tool
extends EditorPlugin

var plugin

func _enable_plugin() -> void:
	pass


func _disable_plugin() -> void:
	pass

func _enter_tree():
	plugin = preload("res://addons/HugosChatPlugin/hugos_chat_plugin.gd").new()
	add_inspector_plugin(plugin)


func _exit_tree():
	remove_inspector_plugin(plugin)
