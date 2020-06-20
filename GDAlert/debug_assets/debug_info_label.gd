extends RichTextLabel


var mouse_position := Vector2(0, 0)
var last_click_position := Vector2(0, 0)
var viewport_rect := Rect2(0, 0, 0, 0)


func _process(delta):
	var mouse_x := int(mouse_position.x)
	var mouse_y := int(mouse_position.y)
	var click_x := int(last_click_position.x)
	var click_y := int(last_click_position.y)
	if delta != 0:
		text = "FPS: " + str(Engine.get_frames_per_second())
		text += "\nMouse Position: " + str(mouse_x) + ", " + str(mouse_y)
		text += "\nLast Click: " + str(click_x) + ", " + str(click_y)

func _on_mouse_moved(position):
	mouse_position = position

func _on_mouse_clicked(position):
	last_click_position = position
