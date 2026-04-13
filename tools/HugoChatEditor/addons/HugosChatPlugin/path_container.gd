@tool
extends HBoxContainer
class_name PathContainer
signal on_path_changed

@export var filters:PackedStringArray:
	set(value):
		filters = value
		file_dialog.clear_filters()
		file_dialog.filters = filters

@export var is_file_path:FileDialog.FileMode:
	set(value):
		is_file_path = value
		file_dialog.file_mode = value

@export var path_button_icon:Texture2D:
	set(value):
		path_button_icon = value
		path_button.icon = value

@export var path_label:String:
	set(value):
		path_label = value
		file_label.text = value

@export_category("REFERENCES")
@export var file_label:Label
@export var file_dialog:FileDialog
@export var path_button:Button
@export var path_text:LineEdit

const PATH_OFFSET = "../../../../../"

func _ready() -> void:
	path_button.button_down.connect(open_file_dialog)
	path_text.text_changed.connect(change_path)
	
	if file_dialog.file_mode == file_dialog.FileMode.FILE_MODE_OPEN_DIR:
		file_dialog.dir_selected.connect(select_directory)
	else:
		file_dialog.file_selected.connect(select_directory)

func open_file_dialog():
	file_dialog.show()

func select_directory(directory:String):
	path_text.text = directory
	change_path(directory)

func change_path(new_text:String):
	on_path_changed.emit(new_text)
