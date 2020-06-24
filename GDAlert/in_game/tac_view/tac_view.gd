extends TextureRect
# Communicates with the native binding to render out the game view and handle IO


signal mouse_moved(pos)
signal mouse_clicked(pos)
signal texture_resized(size)
signal sidebar_state_updated(state)

export var game_speed := 1.0 setget set_game_speed
export var sfx_volume_db := -15.0

var game_palette_image := Image.new()
var game_palette_texture := ImageTexture.new()
var has_selected := false


func _ready():
	Engine.iterations_per_second = int(30 / game_speed)

	# Create placeholder image before the in-game framebuffer is read
	var game_image := Image.new()
	game_image.create(256, 256, false, Image.FORMAT_L8)
	game_image.fill(Color(1, 0, 0, 1))

	# Initialize native game code
	texture = RedAlert.new() as ImageTexture
	texture.create_from_image(game_image, 0)

	# Connect signals for events incoming from native code
	var _error := texture.connect("event_not_handled", self, "_debug_message")
	_error = texture.connect("sound_played", self, "_play_sound")
	_error = texture.connect("speech_played", self, "_play_speech")

	# Start an instance of a scenario
	# RedAlert.start_instance(scenario_number, build_level, "ALLY" or "USSR")
	texture.start_instance(1, 10, "ALLY")

	# Create plateholder palette texture for depalettize shader
	game_palette_image.create(256, 1, false, Image.FORMAT_RGB8)
	game_palette_texture.create_from_image(game_palette_image, 0)

	# Connect palette texture to depalettize shader
	material.set_shader_param("palette_tex", game_palette_texture)


func _physics_process(_delta):
	# Progress game logic by one tick
	texture.advance_instance(0)

	# Read in state of game objects
	var objects: Array = texture.get_game_objects()
	$DebugOverlay.draw_things(objects)

	# Update depalettize shader palette texture
	var palette: PoolByteArray = texture.get_palette()
	game_palette_image.data.data = palette
	game_palette_texture.set_data(game_palette_image)

	# Update legacy framebuffer
	var _got_data: bool = texture.get_visible_page()
	emit_signal("texture_resized", texture.get_size())

	# Update sidebar state
	var sidebar_state: Dictionary = texture.get_sidebar_state()
	emit_signal("sidebar_state_updated", sidebar_state)


func _gui_input(event):
	if event is InputEventMouseButton:
		var x: float = event.position.x
		var y: float = event.position.y
		if event.button_index == BUTTON_LEFT:
			if !event.pressed:
				if !has_selected:
					texture.handle_left_mouse_up(x, y)
					emit_signal("mouse_clicked", Vector2(x, y))
				else:
					has_selected = false
		if event.button_index == BUTTON_RIGHT:
			if event.pressed:
				texture.handle_right_mouse_down(x, y)
			else:
				texture.handle_right_mouse_up(x, y)

		# Update cursor sprite
		var cursor_name: String = texture.get_cursor_name(x, y)
		GameCursor.set_cursor(cursor_name)

	if event is InputEventMouseMotion:
		var mouse_pos: Vector2 = event.position
		texture.handle_mouse_motion(mouse_pos.x, mouse_pos.y)

		var cursor_name: String = texture.get_cursor_name(mouse_pos.x, mouse_pos.y)
		GameCursor.set_cursor(cursor_name)

		emit_signal("mouse_moved", event.position)


func set_game_speed(value: float):
	game_speed = value
	Engine.iterations_per_second = int(30 / value)


func _on_select_rect_finish(p1, p2):
	texture.handle_mouse_area(p1.x, p1.y, p2.x, p2.y)
	has_selected = true


# Prints unhandled events
func _debug_message(msg):
	print_debug(msg)


# Plays announcer sound effects
func _play_speech(event):
	var name: String = event["speech_name"]

	# Load sound effect from legacy game content
	var sample := AudioStreamAud.new()
	sample.name = name

	var player := AudioStreamPlayer.new()
	add_child(player)
	player.set_stream(sample)
	player.volume_db = sfx_volume_db;
	player.play()
	yield(player, "finished")
	player.queue_free()


# Plays game sound effects
func _play_sound(event):
	var name: String = event["sound_effect_name"]
	var position: Vector2 = event["position"]

	# Load sound effect from legacy game content
	var sample := AudioStreamAud.new()
	sample.name = name

	# The game sets the position to (-1, -1) for non-positional SFX
	if position.x < 0 or position.y < 0:
		var player := AudioStreamPlayer.new()
		add_child(player)
		player.set_stream(sample)
		player.volume_db = sfx_volume_db;
		player.play()
		yield(player, "finished")
		player.queue_free()
	else:
		var player := AudioStreamPlayer2D.new()
		add_child(player)
		player.set_stream(sample)
		player.position = position
		player.volume_db = sfx_volume_db;
		player.play()
		yield(player, "finished")
		player.queue_free()


func _on_TacView_mouse_entered():
	GameCursor.cursor_in_tacview = true


func _on_TacView_mouse_exited():
	GameCursor.cursor_in_tacview = false
