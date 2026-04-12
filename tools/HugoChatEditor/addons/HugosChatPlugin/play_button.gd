@tool
extends Button
class_name PlayButton

func _ready() -> void:
	button_down.connect(play)

func play():
	print("Ejecutando juego")
	OS.execute(ProjectSettings.get_setting("hugo/exe_path"), PackedStringArray(ProjectSettings.get_setting("hugo/program_args")))
