class_name FormatWriter

static func format(variant: Variant, name: String):
	if variant is Vector2:
		return "VECTOR_2 %s %s %s" % [name, variant.x, variant.y]
	elif variant is Vector3:
		return "VECTOR_3 %s %s %s %s" % [name, variant.x, variant.y, variant.z]
	elif variant is Vector4:
		return "VECTOR_4 %s %s %s %s %s" % [name, variant.x, variant.y, variant.z, variant.w]
	elif variant is Quaternion:
		return "QUATERNION %s %s %s %s %s" % [name, variant.x, variant.y, variant.z, variant.w]
	elif variant is Mesh:
		return "MESH %s %s" % [name, variant.resource_path]
	elif variant is PackedScene:
		return "PREFAB %s %s" % [name, variant.resource_path]
	elif variant is String:
		return "STRING %s %s" % [name, variant]
	return "%s %s %s" % [ type_string(typeof(variant)).to_upper(), name, str(variant)]
