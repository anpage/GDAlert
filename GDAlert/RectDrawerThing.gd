extends Control


func draw_things(things):
	for n in get_children():
		remove_child(n)
		n.queue_free()
	for thing in things:
		var rect = ColorRect.new()
		rect.color = Color(1, 0, 0, 0.2)
		rect.set_position(Vector2(thing["CenterCoordX"] - thing["DimensionX"] / 2, thing["CenterCoordY"] - thing["DimensionY"] /2))
		rect.set_size(Vector2(thing["DimensionX"], thing["DimensionY"]))
		rect.mouse_filter = MOUSE_FILTER_IGNORE
		add_child(rect)
