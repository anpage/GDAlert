extends Control


var is_mouse_down = false
var is_selecting = false
var rect = Rect2(0, 0, 0, 0)

signal select_rect_finish(p1, p2)

func _ready():
	$ColorRect.hide()

func _gui_input(event):
	if event is InputEventMouseButton:
		var x = event.position.x
		var y = event.position.y
		if event.button_index == BUTTON_LEFT:
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
				$ColorRect.hide()
				is_mouse_down = false
	if event is InputEventMouseMotion:
		if is_mouse_down:
			rect.size.x = event.position.x - rect.position.x
			rect.size.y = event.position.y - rect.position.y
			$ColorRect.rect_size = rect.abs().size
			$ColorRect.rect_position = rect.abs().position
			if !is_selecting:
				if abs(rect.size.x) >= 4 or abs(rect.size.y) >= 4:
					is_selecting = true
					$ColorRect.show()
