class_name FormatWriter

static func format(variant:Variant, name:String):
	if variant is Vector2:
		return "VECTOR_2 " + name + " " + str(variant.x) + " " + str(variant.y)
	elif variant is Vector3:
		return "VECTOR_3 " + name + " " + str(variant.x) + " " + str(variant.y) + " " + str(variant.z)
	elif variant is Vector4:
		return "VECTOR_4 " + name + " " + str(variant.x) + " " + str(variant.y) + " " + str(variant.z) + " " + str(variant.w)
	elif variant is Mesh:
		return "MESH " + name + " " + variant.resource_path
	elif variant is PackedScene:
		return "PREFAB " + name + " " + variant.resource_path
	elif  variant is String:
		return "STRING " + name + " " + variant
	
	return type_string(typeof(variant)).to_upper() + " " + str(variant)
