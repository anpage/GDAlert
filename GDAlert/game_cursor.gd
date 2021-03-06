extends Node
# Manages cursor graphics


# Data file that contains metadata about cursor textures
export var data_file_path := "res://mouse_default.json"
export var cursor_in_tacview := false setget set_cursor_in_tacview

var _mouse_control: Dictionary
var _textures: Dictionary
var _is_loaded := false
var _framerate := 0
var _frames := 0
var _frame := 0
var _hotspot := Vector2(0, 0)
var _texture: AnimatedTexture
var _time_since_update := 0.0
var _cursor_name: String


func _ready():
	_texture = CursorTexture.new()
	var f := File.new()
	var err := f.open(data_file_path, File.READ)

	if not err:
		_mouse_control = JSON.parse(f.get_as_text()).result

		_load_cursors()
	else:
		print("Error loading cursor data file: %d" % err)

	f.close()


func _process(delta):
	if _framerate != 0 and _is_loaded:
		_time_since_update += delta
		if _time_since_update >= 1.0 / _framerate:
			var cursor_texture := _texture.get_frame_texture(_frame)
			Input.set_custom_mouse_cursor(cursor_texture, 0, _hotspot)
			_frame += 1
			if _frame >= _frames:
				_frame = 0
			_time_since_update = 0


func set_cursor(name):
	if not cursor_in_tacview:
		return

	if _is_loaded and name != _cursor_name:
		_cursor_name = name
		var control_data: Dictionary = _mouse_control["control_data"]
		var data: Dictionary = control_data[name]
		_frames = data["frame_count"]
		# Double the framerate; default is too slow
		_framerate = data["frame_rate"] * 2
		_hotspot = Vector2(data["hspot_x"] * 2, data["hspot_y"] * 2)
		_texture = _textures[name]
		_frame = 1
		_time_since_update = 0

		var cursor_texture := _texture.get_frame_texture(0)
		Input.set_custom_mouse_cursor(cursor_texture, 0, _hotspot)


func set_cursor_in_tacview(value):
	if not value:
		set_cursor("MOUSE_NORMAL")
	cursor_in_tacview = value


func _load_cursors():
	var control_data: Dictionary = _mouse_control["control_data"]
	for k in control_data.keys():
		var data: Dictionary = control_data[k]
		var start_frame: int = data["start_frame"]
		var frame_count: int = data["frame_count"]
		var new_texture = CursorTexture.new()
		new_texture.load_cursor_texture(_mouse_control["filename"], start_frame, frame_count)
		_textures[k] = new_texture

	_is_loaded = true

	set_cursor("MOUSE_NORMAL")
