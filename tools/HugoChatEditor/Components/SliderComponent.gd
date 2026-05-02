@tool
extends Component
class_name SliderComponent

@export var trackElementName:String 
@export var knobElementName:String 
@export var minValue:float
@export var maxValue:float
@export var currentValue:float
@export var knobWidth:float
@export var knobHeight:float
@export var knobLeft:float
@export var knobTop:float
@export var trackMaterial:String 
@export var knobMaterial:String 
@export var orientation:ENUMS.SliderOrientation 

func get_component() -> Dictionary[String, Variant]:
	return {
		"minValue" : FormatWriter.format(minValue, "minValue"),
		"maxValue" : FormatWriter.format(maxValue, "maxValue"),
		"currentValue" : FormatWriter.format(currentValue, "currentValue"),
		"knobWidth" : FormatWriter.format(knobWidth, "knobWidth"),
		"knobHeight" : FormatWriter.format(knobHeight, "knobHeight"),
		"knobLeft" : FormatWriter.format(knobLeft, "knobLeft"),
		"knobTop" : FormatWriter.format(knobTop, "knobTop"),
		"trackMaterial" : FormatWriter.format(trackMaterial, "trackMaterial"),
		"knobMaterial" : FormatWriter.format(knobMaterial, "knobMaterial"),
		"orientation" : FormatWriter.format(orientation, "orientation"),
	}
