extends Component
class_name ViewportComponent

@export var left:float
@export var top:float
@export var width:float
@export var height:float
@export var zOrder:int
@export var clearColor:Vector4

func get_component() -> Dictionary[String, Variant]:
	return {
		"left" : left,
		"top" : top,
		"width" : width,
		"height" : height,
		"zOrder" : zOrder,
		"clearColor" : clearColor,
	}
