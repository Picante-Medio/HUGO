extends Area2D
class_name PapingArea

@onready var project_manager:HugoProjectManager = get_tree().get_first_node_in_group(&"HUGO_PROJECT_MANAGER")
@export var animation:AnimationPlayer
var current_cd:Draggable

func _ready() -> void:
	body_entered.connect(enter_cd)
	body_exited.connect(exit_cd)

func enter_cd(cd):
	if cd is not Draggable: return
	current_cd = cd
	current_cd.on_drop.connect(papear_cd)
	animation.play(&"OpenMouth")

func exit_cd(_cd:Draggable):
	if not current_cd: return
	current_cd.on_drop.disconnect(papear_cd)
	animation.play(&"CloseMouth")
	current_cd = null

func papear_cd():
	animation.play(&"CloseMouth")
	remove_dir(current_cd.get_project_path())
	project_manager.remove_project(current_cd.get_project_path())
	SignalBus.kill_cd(current_cd.get_project_path())
	current_cd.queue_free()

func empty_dir(input_dir_path: String):
	var dir = DirAccess.open(input_dir_path)
	remove_dir_files(input_dir_path)
	for i in dir.get_directories():
		var i_path: String=input_dir_path+"/"+i
		remove_dir(i_path)
	return true

func remove_dir(input_dir_path: String):
	empty_dir(input_dir_path)
	DirAccess.remove_absolute(input_dir_path)
	return true

func remove_dir_files(input_dir: String):
	var dir = DirAccess.open(input_dir)
	if not dir: return
	for file in dir.get_files():
		dir.remove(file)
	return true
