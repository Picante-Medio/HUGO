# ClickToDrag.gd
extends RigidBody2D
class_name Draggable

@export var project_name_label:Label
@export var project:Project
var blocked:bool

@warning_ignore("unused_signal")
signal on_drop

func _ready() -> void:
	input_pickable = true

func _input_event(_viewport: Viewport, event: InputEvent, _shape_idx: int) -> void:
	if blocked: return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			ClickJoint.begin_drag(self, get_global_mouse_position())
		else:
			ClickJoint.end_drag()
			on_drop.emit()

func play():
	project.play()

func set_path(path:String):
	var splitted_path:PackedStringArray = path.split("/")
	var project_name:String = splitted_path[splitted_path.size() - 1]
	project_name_label.text = project_name
	project.project_path = path
