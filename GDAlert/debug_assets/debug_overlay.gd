extends Control
# Helps visualize the game state


# Takes in an array of object dicts from the native wrapper
func draw_things(things: Array):
	for n in get_children():
		remove_child(n)
		n.queue_free()
	for thing in things:
		# Draw bounding boxes
		var rect = ColorRect.new()
		rect.rect_size = thing["size"]
		rect.rect_position.x = thing["position"].x - (rect.rect_size.x / 2)
		rect.rect_position.y = thing["position"].y - (rect.rect_size.y / 2)
		rect.color = Color(1, 0, 0, 0.2)
		rect.mouse_filter = Control.MOUSE_FILTER_IGNORE
		add_child(rect)

		# Draw selection "hiboxes"
		if thing["type"] != "BUILDING":
			var shape = CollisionShape2D.new()
			shape.shape = CircleShape2D.new()
			shape.shape.radius = 18
			shape.position = thing["position"]
			add_child(shape)

		for cell in thing["occupied_cells"]:
			var position: Vector2 = cell * 24
			var shape = CollisionShape2D.new()
			shape.shape = CircleShape2D.new()
			shape.shape.radius = 18
			shape.position = Vector2(position.x + 12, position.y + 12)
			add_child(shape)
