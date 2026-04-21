@tool
extends Component
class_name RigidBodyComponent

@export var mass:float
@export var linearDamping:float
@export var angularDamping:float
@export var linearVelocity:Vector3
@export var angularVelocity:Vector3
@export var linearSleepingThreshold:float
@export var angularSleepingThreshold:float

func get_component() -> Dictionary[String, Variant]:
	return {
		"mass" : FormatWriter.format(mass, "mass"),
		"linearDamping" : FormatWriter.format(linearDamping, "linearDamping"),
		"angularDamping" : FormatWriter.format(angularDamping, "angularDamping"),
		"linearVelocity" : FormatWriter.format(linearVelocity, "linearVelocity"),
		"angularVelocity" : FormatWriter.format(angularVelocity, "angularVelocity"),
		"linearSleepingThreshold" : FormatWriter.format(linearSleepingThreshold, "linearSleepingThreshold"),
		"angularSleepingThreshold" : FormatWriter.format(angularSleepingThreshold, "angularSleepingThreshold"),
	}
