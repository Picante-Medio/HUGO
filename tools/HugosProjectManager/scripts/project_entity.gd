extends Node
class_name Project

var project_path: String

func play():
	OS.create_process(get_godot_executable(), ["--editor", project_path + "/project.godot"])
	get_tree().free()

func get_godot_executable() -> String:
	var os_name = OS.get_name()
	var version := "4.6"
	var godots_folder = "Godots/"
	
	# Sufijos comunes en los nombres de ejecutables de Godot
	var suffixes := ["", "-stable", ".stable", "-rc1", "-beta1"]
	
	var candidates: PackedStringArray = []
	for suffix in suffixes:
		var v = version + suffix
		match os_name:
			"Windows":
				candidates.append(godots_folder + "Godot_v%s_win64.exe" % v)
				candidates.append(godots_folder + "Godot_v%s_win64.console.exe" % v)
			"macOS":
				candidates.append(godots_folder + "Godot_v%s.app/Contents/MacOS/Godot" % v)
			"Linux", "FreeBSD", "NetBSD", "OpenBSD", "BSD":
				candidates.append(godots_folder + "Godot_v%s_linux.x86_64" % v)
				candidates.append(godots_folder + "Godot_v%s_linux.arm64" % v)
				candidates.append(godots_folder + "godot")  # instalación del sistema
	
	for candidate in candidates:
		if FileAccess.file_exists(candidate):
			return candidate
	
	push_error("No se encontró ningún ejecutable de Godot para: " + os_name)
	return ""
