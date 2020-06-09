extends TextureRect

onready var redAlert = preload("res://bin/RedAlert.gdns").new()

var page_width = 0
var page_height = 0
var game_image = Image.new()
var has_selected = false
var is_scrolling = false
var scroll_start = Vector2(0, 0)

var time_since_game_tick = 0.0

func debug_message(msg):
	print (msg)

func _ready():
	pass
	redAlert.register_class(self)
	var content_path = ProjectSettings.globalize_path("res://RedAlert")
	var cmdline = "-CD\"" + content_path.replace("/", "\\") + "\""
	redAlert.cnc_init(cmdline)
	# redAlert.cnc_start_instance(scenario_number, build_level, "ALLY" or "USSR")
	redAlert.cnc_start_instance(3, 10, "ALLY")
	game_image.create(256, 256, false, Image.FORMAT_RGBA8)
	game_image.fill(Color(1,0,0,1))
	texture = ImageTexture.new()
	texture.create_from_image(game_image)

func _process(delta):
	time_since_game_tick += delta

	if time_since_game_tick >= 0.03333333333:
		redAlert.cnc_advance_instance(0)
		time_since_game_tick = 0

	var data = redAlert.cnc_get_visible_page()
	var width = redAlert.cnc_get_visible_page_width()
	var height = redAlert.cnc_get_visible_page_height()
	if width != page_width or height != page_height:
		page_width = width
		page_height = height
		game_image.create(width, height, false, Image.FORMAT_RGBA8)
	game_image.data.data = data
	texture.set_data(game_image)
	texture.flags = 0
	texture.set_size_override(Vector2(width, height))

func _gui_input(event):
	if event is InputEventMouseButton:
		var x = event.position.x
		var y = event.position.y
		if event.button_index == BUTTON_LEFT:
			if !event.pressed:
				if !has_selected:
					redAlert.cnc_handle_left_mouse_up(x, y)
				else:
					has_selected = false
		if event.button_index == BUTTON_RIGHT:
			if event.pressed:
				redAlert.cnc_handle_right_mouse_down(x, y)
			else:
				redAlert.cnc_handle_right_mouse_up(x, y)
		if event.button_index == BUTTON_WHEEL_UP and event.pressed:
			$GameCamera.zoom /= 2
			$GameCamera.position += $GameCamera.get_viewport_rect().size * $GameCamera.zoom * 0.5
			clamp_camera_position()
		if event.button_index == BUTTON_WHEEL_DOWN and event.pressed:
			$GameCamera.position -= $GameCamera.get_viewport_rect().size * $GameCamera.zoom * 0.5
			$GameCamera.zoom *= 2
			clamp_camera_position()
		if event.button_index == BUTTON_MIDDLE:
			if event.pressed:
				is_scrolling = true
				scroll_start = event.position
			else:
				is_scrolling = false
	if event is InputEventMouseMotion:
		if is_scrolling:
			var x = $GameCamera.position.x + (scroll_start.x - event.position.x)
			var y = $GameCamera.position.y + (scroll_start.y - event.position.y)
			$GameCamera.position.x = x
			$GameCamera.position.y = y
			clamp_camera_position()


func clamp_camera_position():
	var max_x = page_width - ($GameCamera.get_viewport_rect().size.x * $GameCamera.zoom.x)
	var new_x = clamp($GameCamera.position.x, 0, max(0, max_x))
	var max_y = page_height - ($GameCamera.get_viewport_rect().size.y * $GameCamera.zoom.y)
	var new_y = clamp($GameCamera.position.y, 0, max(0, max_y))
	$GameCamera.position.x = new_x
	$GameCamera.position.y = new_y

func _on_select_rect_finish(p1, p2):
	redAlert.cnc_handle_mouse_area(p1.x, p1.y, p2.x, p2.y)
	has_selected = true
