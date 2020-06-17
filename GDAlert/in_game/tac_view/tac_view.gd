extends TextureRect


signal started_scrolling(pos)
signal stopped_scrolling()
signal mouse_moved(pos)

export var game_speed: float = 1.0

var game_size := Vector2(0, 0)
var game_image := Image.new()
var game_palette_image := Image.new()
var game_palette_texture := ImageTexture.new()
var has_selected := false

var time_since_game_tick := 0.0

onready var RedAlert = preload("res://bin/red_alert.gdns")
onready var AudSample = preload("res://bin/audio_stream_aud.gdns")


func debug_message(msg):
	print_debug(msg)


func play_speech(name: String):
	var sample = AudSample.new()
	sample.name = name

	var player: AudioStreamPlayer = AudioStreamPlayer.new()
	add_child(player)
	player.set_stream(sample)
	player.volume_db = -15;
	player.play()
	yield(player, "finished")
	player.queue_free()


func play_sound(name: String, x: int, y: int):
	var sample = AudSample.new()
	sample.name = name

	var player: AudioStreamPlayer2D = AudioStreamPlayer2D.new()
	add_child(player)
	player.set_stream(sample)
	player.position = Vector2(x, y)
	player.volume_db = -15;
	player.play()
	yield(player, "finished")
	player.queue_free()


func _ready():
	game_image.create(256, 256, false, Image.FORMAT_L8)
	game_image.fill(Color(1, 0, 0, 1))
	texture = RedAlert.new()
	texture.create_from_image(game_image, 0)
	texture.connect("event", self, "debug_message")
	texture.connect("play_sound", self, "play_sound")
	texture.connect("play_speech", self, "play_speech")
	# RedAlert.start_instance(scenario_number, build_level, "ALLY" or "USSR")
	texture.start_instance(1, 10, "ALLY")

	game_palette_image.create(256, 1, false, Image.FORMAT_RGB8)
	game_palette_texture = ImageTexture.new()
	game_palette_texture.create_from_image(game_palette_image)
	game_palette_texture.flags = 0
	material.set_shader_param("palette_tex", game_palette_texture)


func _process(delta):
	time_since_game_tick += delta

	if time_since_game_tick >= (1.0 / 30.0) / game_speed:
		texture.advance_instance(0)
		var objects: Array = texture.get_game_objects()
		$DebugOverlay.draw_things(objects)
		time_since_game_tick = 0

	var palette = texture.get_palette()
	game_palette_image.data.data = palette
	game_palette_texture.set_data(game_palette_image)

	var _got_data = texture.get_visible_page()


func _gui_input(event):
	if event is InputEventMouseButton:
		var x = event.position.x
		var y = event.position.y
		if event.button_index == BUTTON_LEFT:
			if !event.pressed:
				if !has_selected:
					texture.handle_left_mouse_up(x, y)
				else:
					has_selected = false
		if event.button_index == BUTTON_RIGHT:
			if event.pressed:
				texture.handle_right_mouse_down(x, y)
			else:
				texture.handle_right_mouse_up(x, y)
		if event.button_index == BUTTON_MIDDLE:
			if event.pressed:
				emit_signal("started_scrolling", event.position)
			else:
				emit_signal("stopped_scrolling")
	if event is InputEventMouseMotion:
		var mouse_pos: Vector2 = event.position
		texture.handle_mouse_motion(mouse_pos.x, mouse_pos.y)

		var cursor_name = texture.get_cursor_name(mouse_pos.x, mouse_pos.y)
		GameCursor.set_cursor(cursor_name)

		emit_signal("mouse_moved", event.position)


func _on_select_rect_finish(p1, p2):
	texture.handle_mouse_area(p1.x, p1.y, p2.x, p2.y)
	has_selected = true
