extends Camera2D


export var max_bounds := Vector2(0, 0)

var _is_scrolling := false
var _scroll_start := Vector2(0, 0)


func _ready():
	pass # Replace with function body.


func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_WHEEL_UP and event.pressed:
			zoom /= 2
			position += get_viewport_rect().size * zoom * 0.5
			_clamp_camera_position()
		if event.button_index == BUTTON_WHEEL_DOWN and event.pressed:
			position -= get_viewport_rect().size * zoom * 0.5
			zoom *= 2
			_clamp_camera_position()


func _on_started_scrolling(pos: Vector2):
	_is_scrolling = true
	_scroll_start = pos


func _on_stopped_scrolling():
	_is_scrolling = false


func _on_mouse_moved(pos: Vector2):
	if _is_scrolling:
			var x = position.x + (_scroll_start.x - pos.x)
			var y = position.y + (_scroll_start.y - pos.y)
			position.x = x
			position.y = y
			_clamp_camera_position()


func _clamp_camera_position():
	var max_x = get_parent().rect_size.x - (get_viewport_rect().size.x * zoom.x)
	var new_x = clamp(position.x, 0, max(0, max_x))
	var max_y = get_parent().rect_size.y - (get_viewport_rect().size.y * zoom.y)
	var new_y = clamp(position.y, 0, max(0, max_y))
	position.x = new_x
	position.y = new_y
