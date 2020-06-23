extends Control


var is_mouse_down = false
var is_selecting = false
var rect = Rect2(0, 0, 0, 0)

signal select_rect_finish(p1, p2)


func _draw():
	if (is_selecting):
		draw_rect(rect, Color(1, 1, 1, 0.5))


func _gui_input(event):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			update()
			if event.pressed:
				rect.position = event.position
				rect.size = Vector2(0, 0)
				is_mouse_down = true
			else:
				if (is_selecting):
					var p1 = rect.position
					var p2 = rect.position + rect.size
					emit_signal("select_rect_finish", p1, p2)
				is_selecting = false
				is_mouse_down = false
	if event is InputEventMouseMotion:
		if is_mouse_down:
			rect.size.x = event.position.x - rect.position.x
			rect.size.y = event.position.y - rect.position.y
			if !is_selecting:
				if abs(rect.size.x) >= 4 or abs(rect.size.y) >= 4:
					is_selecting = true
			update()
