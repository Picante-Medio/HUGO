@tool
extends EditorScript
class_name LuaSync

const ENGINE_FUNC_PATHS = ["../../HUGO/src/lua/test.cpp"]
const LUA_PATH = "../../HUGO/bin/test.lua"

func _run() -> void:
	sync()

func sync():
	var functions:Array[Array] = []
	for path_name:String in ENGINE_FUNC_PATHS:
		var functions_file := FileAccess.open(path_name, FileAccess.READ)
		var line := functions_file.get_line().strip_edges()
		
		while not functions_file.eof_reached():
			if line.contains("registerFunc<"):
				var args:PackedStringArray = line.split("registerFunc<")[1].split(">")[0].split(",")
				var function_name:String = line.split('"')[1]
				functions.push_back([function_name, args.size()])
				
			line = functions_file.get_line().strip_edges()
		functions_file.close()
	
	var lua_file := FileAccess.open(LUA_PATH, FileAccess.WRITE)

	for function in functions:
		var text_to_write:String = "function " + function[0] + "("
		for arg in range(0, function[1]):
			text_to_write += "p_" + str(arg)
			if arg != function[1] -1: text_to_write += ", "
			else: text_to_write += ") end"
		lua_file.store_line(text_to_write)

	lua_file.close()
