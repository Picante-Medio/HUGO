@tool
extends Component
class_name ParticleEmitterComponent

@export var particleName:String
@export var quota:int
@export var sorted:bool
@export var direction:Vector3
@export var emissionRate:float
@export var timeToLive:float
@export var velocityMin:float
@export var velocityMax:float
@export var colour:Vector4
@export var duration:float
@export var material:String
@export var width:float
@export var height:float
@export var emitting:bool
@export var active:bool

func get_component() -> Dictionary[String, Variant]:
	return {
		"particleName" : FormatWriter.format(particleName, "particleName"),
		"quota" : FormatWriter.format(quota, "quota"),
		"sorted" : FormatWriter.format(sorted, "sorted"),
		"direction" : FormatWriter.format(direction, "direction"),
		"emissionRate" : FormatWriter.format(emissionRate, "emissionRate"),
		"timeToLive" : FormatWriter.format(timeToLive, "timeToLive"),
		"velocityMin" : FormatWriter.format(velocityMin, "velocityMin"),
		"velocityMax" : FormatWriter.format(velocityMax, "velocityMax"),
		"colour" : FormatWriter.format(colour, "colour"),
		"duration" : FormatWriter.format(duration, "duration"),
		"material" : FormatWriter.format(material, "material"),
		"width" : FormatWriter.format(width, "width"),
		"height" : FormatWriter.format(height, "height"),
		"emitting" : FormatWriter.format(emitting, "emitting"),
		"active" : FormatWriter.format(active, "active"),
	}
