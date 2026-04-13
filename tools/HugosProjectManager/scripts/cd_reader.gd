extends Sprite2D
class_name CDReader

@export var animation:AnimationPlayer
@export var trigger_area:Area2D
@export var reader:Sprite2D

var current_cd:Draggable
var cd_entered:bool

func _ready() -> void:
	trigger_area.body_entered.connect(enter_cd)
	trigger_area.body_exited.connect(exit_cd)

func enter_cd(cd):
	if current_cd: return
	if cd is not Draggable: return
	current_cd = cd
	current_cd.on_drop.connect(play_cd)
	animation.play(&"OpenCD")

func exit_cd(_cd:Draggable):
	if cd_entered: return
	current_cd.on_drop.disconnect(play_cd)
	animation.play(&"CloseCD")
	current_cd = null

func play_cd():
	current_cd.z_index = 1
	current_cd.reparent(reader)
	current_cd.position = Vector2.ZERO
	animation.play(&"CloseCD")
	
	cd_entered = true
	current_cd.blocked = true
	current_cd.freeze = true
	await get_tree().create_timer(1).timeout
	current_cd.play()
