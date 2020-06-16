extends Control

onready var Circle = preload("res://debug_assets/circle.tscn")

func draw_things(things):
	for n in get_children():
		remove_child(n)
		n.queue_free()
	for thing in things:
		for cell in thing["OccupyList"]:
			var circle = Circle.instance()
			circle.set_position(Vector2(cell.x + 12, cell.y + 12))
			add_child(circle)
