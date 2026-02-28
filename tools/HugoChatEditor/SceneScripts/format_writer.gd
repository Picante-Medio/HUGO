class_name FormatWriter

static func format(variant):
	if variant is Vector2:
		return variant.x + " " + variant.y
	elif variant is Vector3:
		return variant.x + " " + variant.y + " " + variant.z
	elif variant is Vector4:
		return variant.x + " " + variant.y + " " + variant.z + " " + variant.w
		
	return variant
