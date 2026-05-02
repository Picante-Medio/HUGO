@tool
extends Component
class_name ProgressBarComponent

@export var fillElementName:String
@export var minValue:float
@export var maxValue:float
@export var currentValue:float
@export var fillMaterial:String
@export var fillMode:ENUMS.ProgressBarFillMode

func get_component() -> Dictionary[String, Variant]:
	return {
		"fillElementName" : FormatWriter.format(fillElementName, "fillElementName"),
		"minValue" : FormatWriter.format(minValue, "minValue"),
		"maxValue" : FormatWriter.format(maxValue, "maxValue"),
		"currentValue" : FormatWriter.format(currentValue, "currentValue"),
		"fillMaterial" : FormatWriter.format(fillMaterial, "fillMaterial"),
		"fillMode" : FormatWriter.format(fillMode, "fillMode"),
	}
