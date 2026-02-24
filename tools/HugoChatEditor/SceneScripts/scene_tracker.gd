@icon("res://Icons/scene_tracker_icon.png")
@tool
extends Node
class_name SCENE_TRACKER

@export var entities:Dictionary[String, Dictionary]
@export var systems:Array[SYSTEMS]

enum SYSTEMS {
	MOVEMENT_SYSTEM,
	CHAT_SYSTEM,
	ZAPAS_SYSTEM,
	SKIBIDI_SYSTEM,
	HUGO_SYSTEM
}
