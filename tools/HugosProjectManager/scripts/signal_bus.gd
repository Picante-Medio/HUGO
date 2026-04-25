extends Node

signal on_cd_killed
signal on_cd_grab
signal on_cd_drop

func kill_cd(project_path:String):
	on_cd_killed.emit(project_path)

func grab_cd(project_path:String):
	on_cd_grab.emit(project_path)

func drop_cd():
	on_cd_drop.emit()
