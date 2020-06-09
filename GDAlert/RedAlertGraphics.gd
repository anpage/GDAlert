extends TextureRect

onready var redAlert = preload("res://bin/RedAlert.gdns").new()

var page_width = 0
var page_height = 0
var game_image = Image.new()
var has_selected = false

var time_since_game_tick = 0.0

func debug_message(msg):
	print (msg)

func _ready():
	pass
	redAlert.register_class(self)
	var content_path = ProjectSettings.globalize_path("res://RedAlert")
	var cmdline = "-CD\"" + content_path.replace("/", "\\") + "\""
	redAlert.cnc_init(cmdline)
	redAlert.cnc_start_instance()
	game_image.create(256, 256, false, Image.FORMAT_RGBA8)
	game_image.fill(Color(1,0,0,1))
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

func _on_select_rect_finish(p1, p2):
	redAlert.cnc_handle_mouse_area(p1.x, p1.y, p2.x, p2.y)
	has_selected = true
