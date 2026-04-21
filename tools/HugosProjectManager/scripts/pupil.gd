extends Sprite2D
class_name Pupil

@export var eye_radius:float = 10.0
@export var master:Node

func _ready() -> void:
	if not master: return
	if master is Draggable: SignalBus.on_cd_killed.connect(_on_cd_killed)
	
	if master is Papeador:
		SignalBus.on_cd_grab.connect(_on_cd_grab)
		SignalBus.on_cd_drop.connect(_restore_pupil)

func _process(_delta: float) -> void:
	var original_pos:Vector2 = get_parent().global_position
	var mouse_pos := get_global_mouse_position()
	var look_dir := (mouse_pos - global_position).normalized()
	
	if mouse_pos.distance_to(original_pos) < eye_radius:
		global_position = mouse_pos
	else:
		global_position = original_pos + look_dir * eye_radius

func _on_cd_killed(_project_path:String):
	var tween := get_tree().create_tween()
	tween.tween_property(self, "scale", Vector2(0.6, 0.6), 0.4)

func _on_cd_grab(_project_path:String):
	var tween := get_tree().create_tween()
	tween.tween_property(self, "scale", Vector2(1.2, 1.2), 0.4)

func _restore_pupil():
	var tween := get_tree().create_tween()
	tween.tween_property(self, "scale", Vector2(1, 1), 0.4)
