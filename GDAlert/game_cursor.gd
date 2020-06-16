extends Node

onready var ShapeTexture = preload("res://bin/shape_texture.gdns")

var mouse_control: Dictionary
var textures: Dictionary = {}

var is_loaded: bool = false

var framerate: int = 0
var frames = 0
var frame = 0
var hotspot: Vector2 = Vector2(0, 0)
var texture: AnimatedTexture
var time_since_update: float = 0.0
var cursor_name: String


func _ready():
	texture = ShapeTexture.new()
	var f = File.new()
	f.open("res://mouse_default.json", File.READ)
	mouse_control = JSON.parse(f.get_as_text()).result
	f.close()


func load_cursors():
	var control_data: Dictionary = mouse_control["control_data"]
	for k in control_data.keys():
		var data: Dictionary = control_data[k]
		var start_frame: int = data["start_frame"]
		var frame_count = data["frame_count"]
		var new_texture = ShapeTexture.new()
		new_texture.load_cursor_texture(mouse_control["filename"], start_frame, frame_count)
		textures[k] = new_texture

	is_loaded = true

	set_cursor("MOUSE_NORMAL")


func set_cursor(name):
	if is_loaded and name != cursor_name:
		cursor_name = name
		var control_data: Dictionary = mouse_control["control_data"]
		var data: Dictionary = control_data[name]
		frames = data["frame_count"]
		framerate = data["frame_rate"] * 2
		hotspot = Vector2(data["hspot_x"] * 2, data["hspot_y"] * 2)
		texture = textures[name]
		frame = 1
		time_since_update = 0

		var cursor_texture = texture.get_frame_texture(0)
		Input.set_custom_mouse_cursor(cursor_texture, 0, hotspot)


func _process(delta):
	if framerate != 0 and is_loaded:
		time_since_update += delta
		if time_since_update >= 1.0 / framerate:
			var cursor_texture = texture.get_frame_texture(frame)
			Input.set_custom_mouse_cursor(cursor_texture, 0, hotspot)
			frame += 1
			if frame >= frames:
				frame = 0
			time_since_update = 0
