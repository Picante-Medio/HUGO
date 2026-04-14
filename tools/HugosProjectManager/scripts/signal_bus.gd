extends Node

signal on_cd_killed

func kill_cd(project_path:String):
	on_cd_killed.emit(project_path)
