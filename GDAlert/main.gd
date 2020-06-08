extends Control

onready var redAlert = preload("res://bin/RedAlert.gdns").new()

func debug_message(msg):
	print (msg)

func _ready():
	pass
	redAlert.register_class(self)
	var content_path = ProjectSettings.globalize_path("res://RedAlert")
	var cmdline = "-CD\"" + content_path.replace("/", "\\") + "\""
	redAlert.cnc_init(cmdline)
	redAlert.cnc_start_instance()

func _process(delta):
	redAlert.cnc_advance_instance(0)

func _on_Button_pressed():
	$Label.text = "Data = " + redAlert.get_data()
