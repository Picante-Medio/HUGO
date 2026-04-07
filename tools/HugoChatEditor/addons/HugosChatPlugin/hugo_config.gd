@tool
extends Control
class_name HugoConfig

static var COMPONENTS_PATH:String
static var CHATS_PATH:String
static var LUA_PATH:String

@export var components_path_container:PathContainer
@export var chats_path_container:PathContainer
@export var lua_path_container:PathContainer

func _ready() -> void:
	components_path_container.on_path_changed.connect(func(value): COMPONENTS_PATH = value)
	chats_path_container.on_path_changed.connect(func(value): CHATS_PATH = value)
	lua_path_container.on_path_changed.connect(func(value): LUA_PATH = value)
	
