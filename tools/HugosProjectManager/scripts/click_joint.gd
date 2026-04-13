# DragJoint2D.gd
extends Node2D

var _anchor: StaticBody2D
var _joint: PinJoint2D
var _held: RigidBody2D

func _ready() -> void:
	_anchor = StaticBody2D.new()
	add_child(_anchor)
	_anchor.collision_layer = 0
	_anchor.collision_mask = 0

func begin_drag(body: RigidBody2D, world_point: Vector2) -> void:
	if _held:
		end_drag()
	_held = body

	# Coloca el “ancla” y el joint en el punto del clic:
	_anchor.global_position = world_point
	_joint = PinJoint2D.new()
	_joint.global_position = world_point            # <— el anclaje es la posición del joint
	_joint.node_a = _held.get_path()                # NodePath al cuerpo
	_joint.node_b = _anchor.get_path()              # NodePath al ancla estática
	# Ajuste opcional de rigidez de la restricción:
	_joint.bias = 0.8
	add_child(_joint)

func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.is_released():
			end_drag()

func end_drag() -> void:
	if _joint and is_instance_valid(_joint):
		_joint.queue_free()
	_joint = null
	_held = null

func _physics_process(_delta: float) -> void:
	if _held:
		_anchor.global_position = get_global_mouse_position()
