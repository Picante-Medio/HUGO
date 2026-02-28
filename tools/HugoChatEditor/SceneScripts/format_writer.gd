class_name FormatWriter

static func format(variant:Variant):
	if variant is Vector2:
		return "VECTOR_2 " + str(variant.x) + " " + str(variant.y)
	elif variant is Vector3:
		return "VECTOR_3 " + str(variant.x) + " " + str(variant.y) + " " + str(variant.z)
	elif variant is Vector4:
		return "VECTOR_4 " + str(variant.x) + " " + str(variant.y) + " " + str(variant.z) + " " + str(variant.w)
	elif variant is Mesh:
		return "MESH " + variant.resource_path
	elif variant is PackedScene:
		return "PREFAB " + variant.resource_path
	
	return type_string(typeof(variant)) + " " + str(variant)
