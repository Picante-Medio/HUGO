@tool
extends Component
class_name TestComponent

@export var testVar:float
@export var stringDePrueba:String

func get_component() -> Dictionary[String, Variant]:
	return {
		"testVar" : FormatWriter.format(testVar, "testVar"),
		"stringDePrueba" : FormatWriter.format(stringDePrueba, "stringDePrueba"),
	}
