@tool
extends Component
class_name ProgressBarComponent

@export var minValue:float
@export var maxValue:float
@export var currentValue:float
@export var fillMaterial:String

func get_component() -> Dictionary[String, Variant]:
	return {
		"minValue" : FormatWriter.format(minValue, "minValue"),
		"maxValue" : FormatWriter.format(maxValue, "maxValue"),
		"currentValue" : FormatWriter.format(currentValue, "currentValue"),
		"fillMaterial" : FormatWriter.format(fillMaterial, "fillMaterial"),
	}
