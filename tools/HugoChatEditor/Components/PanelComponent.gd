@tool
extends Component
class_name PanelComponent

@export var overlayName:String
@export var containerName:String
@export var isVisible:bool
@export var metricsMode:ENUMS.UIMetricsMode
@export var left:float
@export var top:float
@export var width:float
@export var height:float
@export var material:String

func get_component() -> Dictionary[String, Variant]:
	return {
		"overlayName" : FormatWriter.format(overlayName, "overlayName"),
		"containerName" : FormatWriter.format(containerName, "containerName"),
		"isVisible" : FormatWriter.format(isVisible, "isVisible"),
		"metricsMode" : FormatWriter.format(metricsMode, "metricsMode"),
		"left" : FormatWriter.format(left, "left"),
		"top" : FormatWriter.format(top, "top"),
		"width" : FormatWriter.format(width, "width"),
		"height" : FormatWriter.format(height, "height"),
		"material" : FormatWriter.format(material, "material"),
	}
