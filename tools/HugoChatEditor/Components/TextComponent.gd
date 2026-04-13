@tool
extends Component
class_name TextComponent

@export var text:String
@export var textFont:String
@export var characterHeight:float
@export var spaceWidth:float
@export var colour:Vector4
@export var colourTop:Vector4
@export var colourBottom:Vector4
@export var left:float
@export var top:float
@export var maxWidth:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"text" : FormatWriter.format(text, "text"),
		"textFont" : FormatWriter.format(textFont, "textFont"),
		"characterHeight" : FormatWriter.format(characterHeight, "characterHeight"),
		"spaceWidth" : FormatWriter.format(spaceWidth, "spaceWidth"),
		"colour" : FormatWriter.format(colour, "colour"),
		"colourTop" : FormatWriter.format(colourTop, "colourTop"),
		"colourBottom" : FormatWriter.format(colourBottom, "colourBottom"),
		"left" : FormatWriter.format(left, "left"),
		"top" : FormatWriter.format(top, "top"),
		"maxWidth" : FormatWriter.format(maxWidth, "maxWidth"),
	}
