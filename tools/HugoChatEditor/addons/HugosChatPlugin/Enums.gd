class_name ENUMS

enum AudioTypes
{
	SOUND2D,
	SOUND3D,
	MUSIC,
	LAST_TYPE
};

enum PolygonMode
{
	SOLID,
	WIREFRAME,
	POINTS
};

enum ProjectionType
{
	Perspective,
	Orthographic
};

enum LightType
{
	LT_POINT,
	LT_DIRECTIONAL,
	LT_SPOTLIGHT,
	LT_RECTLIGHT
};

enum ButtonStateTypes
{
	BS_UP,
	BS_OVER,
	BS_DOWN
};

enum UIHorizontalAlignment
{
	UHA_LEFT,
	UHA_CENTER,
	UHA_RIGHT
};

enum UIVerticalAlignment
{
	UVA_TOP,
	UVA_CENTER,
	UVA_BOTTOM
};

enum UIMetricsMode
{
	GMM_RELATIVE,
	GMM_PIXELS,
	GMM_RELATIVE_ASPECT_ADJUSTED
};

enum ProgressBarFillMode {
	HORIZONTAL_LEFT_TO_RIGHT,
	HORIZONTAL_RIGHT_TO_LEFT,
	VERTICAL_BOTTOM_TO_TOP, 
	VERTICAL_TOP_TO_BOTTOM,
};

enum SliderOrientation {
	HORIZONTAL,
	VERTICAL
};

enum TrayLocationTypes
{
	TL_TOPLEFT,
	TL_TOP,
	TL_TOPRIGHT,
	TL_LEFT,
	TL_CENTER,
	TL_RIGHT,
	TL_BOTTOMLEFT,
	TL_BOTTOM,
	TL_BOTTOMRIGHT,
	TL_NONE
};

enum ColliderType 
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER,
	CONE
};

const TYPES_DICTIONARY = { "std::string": "String", "Bool": "bool", "Int": "int", "Float": "float", "Double": "float", "AudioID": "int", "SceneID": "int", "AudioEndCallback": "StringName", "CollisionObjectID": "int", "CollisionShapeID": "int", "double" : "float", "Radian" : "float" }
