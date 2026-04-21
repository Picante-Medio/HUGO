extends Button

func _pressed() -> void:
	ScriptsSyncronizer.sync()
