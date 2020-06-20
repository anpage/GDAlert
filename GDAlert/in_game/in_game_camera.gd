extends Camera2D


export var max_bounds := Vector2(0, 0)

var _is_scrolling := false
var _scroll_start := Vector2(0, 0)
var _sidebar_width := 120.0


func _ready():
	limit_left = 0
	limit_top = 0

func _process(delta):
	if _is_scrolling:
		var mouse_pos := get_viewport().get_mouse_position()
		var nextpos := position
		nextpos.x += min((mouse_pos.x - _scroll_start.x) * delta * zoom.x * 8, 5)
		nextpos.y += min((mouse_pos.y - _scroll_start.y) * delta * zoom.y * 8, 5)
		
		var min_x = (get_viewport_rect().size.x * zoom.x) / 2
		var max_x = limit_right - (get_viewport_rect().size.x * zoom.x) / 2
		
		var min_y = (get_viewport_rect().size.y * zoom.y) / 2
		var max_y = limit_bottom - (get_viewport_rect().size.y * zoom.y) / 2

		# Godot doesn't auto clamp the camera's position value to the limit
		if nextpos.x > min_x:
			if nextpos.x < max_x:
				position.x = nextpos.x
			else:
				position.x = max_x
		else:
			position.x = min_x

		if nextpos.y > min_y:
			if nextpos.y < max_y:
				position.y = nextpos.y
			else:
				position.y = max_y
		else:
			position.y = min_y


func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_WHEEL_UP and event.pressed:
			$ZoomTween.interpolate_property(self, "zoom", zoom, zoom / 2, 0.1,
					Tween.TRANS_LINEAR, Tween.EASE_IN_OUT)
			$ZoomTween.start()
		if event.button_index == BUTTON_WHEEL_DOWN and event.pressed:
			$ZoomTween.interpolate_property(self, "zoom", zoom, zoom * 2, 0.1,
					Tween.TRANS_LINEAR, Tween.EASE_IN_OUT)
			$ZoomTween.start()
		if event.button_index == BUTTON_MIDDLE:
			if event.is_pressed():
				_is_scrolling = true
				_scroll_start = event.position
			else:
				_is_scrolling = false


func set_bounds(limit: Vector2):
	limit.x += (_sidebar_width * zoom.x)
	limit_right = int(max(limit.x, get_viewport_rect().size.x * zoom.x))
	limit_bottom = int(max(limit.y, get_viewport_rect().size.y * zoom.y))


func set_sidebar_width(sidebar_width: float):
	_sidebar_width = sidebar_width


func _clamp_camera_position():
	var max_x = get_parent().rect_size.x - (get_viewport_rect().size.x * zoom.x)
	var new_x = clamp(position.x, 0, max(0, max_x))
	var max_y = get_parent().rect_size.y - (get_viewport_rect().size.y * zoom.y)
	var new_y = clamp(position.y, 0, max(0, max_y))
	position.x = new_x
	position.y = new_y
