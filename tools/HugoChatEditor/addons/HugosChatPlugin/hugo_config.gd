@tool
extends Control
class_name HugoConfig

static var COMPONENTS_PATH:String
static var CHATS_PATH:String
static var LUA_PATH:String
static var SYSTEMS_PATH:String

@export var components_path_container:PathContainer
@export var chats_path_container:PathContainer
@export var lua_path_container:PathContainer
@export var systems_path_container:PathContainer

func _ready() -> void:
	COMPONENTS_PATH = ProjectSettings.get_setting("hugo/components_path", "MyComponents")
	CHATS_PATH = ProjectSettings.get_setting("hugo/chats_path", "Chats")
	LUA_PATH = ProjectSettings.get_setting("hugo/lua_path", "")
	SYSTEMS_PATH = ProjectSettings.get_setting("hugo/systems_path", "Systems")
	
	ProjectSettings.set_setting("hugo/components_path", COMPONENTS_PATH)
	ProjectSettings.set_setting("hugo/chats_path", CHATS_PATH)
	ProjectSettings.set_setting("hugo/lua_path", LUA_PATH)
	ProjectSettings.set_setting("hugo/systems_path", SYSTEMS_PATH)
	
	components_path_container.path_text.text = COMPONENTS_PATH
	chats_path_container.path_text.text = CHATS_PATH
	lua_path_container.path_text.text = LUA_PATH
	systems_path_container.path_text.text = SYSTEMS_PATH
	
	components_path_container.on_path_changed.connect(set_components_path)
	chats_path_container.on_path_changed.connect(set_chats_path)
	lua_path_container.on_path_changed.connect(set_lua_path)
	systems_path_container.on_path_changed.connect(set_systems_path)

func set_systems_path(value):
	SYSTEMS_PATH = value
	ProjectSettings.set_setting("hugo/systems_path", value)

func set_components_path(value):
	COMPONENTS_PATH = value
	ProjectSettings.set_setting("hugo/components_path", value)


func set_chats_path(value):
	CHATS_PATH = value
	ProjectSettings.set_setting("hugo/chats_path", value)


func set_lua_path(value):
	LUA_PATH = value
	ProjectSettings.set_setting("hugo/lua_path", value)
