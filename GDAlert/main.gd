extends Control

onready var redAlert = preload("res://bin/RedAlert.gdns").new()

var page_width = 0
var page_height = 0
var game_image = Image.new()

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
	$TextureRect.texture.create_from_image(game_image)

func _process(delta):
	if delta != 0:
		$RichTextLabel.text = "FPS: " + str((1/delta))

	redAlert.cnc_advance_instance(0)

	var data = redAlert.cnc_get_visible_page()
	var width = redAlert.cnc_get_visible_page_width()
	var height = redAlert.cnc_get_visible_page_height()
	if width != page_width or height != page_height:
		page_width = width
		page_height = height
		game_image.create(width, height, false, Image.FORMAT_RGBA8)
	game_image.data.data = data
	$TextureRect.texture.set_data(game_image)
	$TextureRect.texture.flags = 0
	$TextureRect.texture.set_size_override(Vector2(width, height))
