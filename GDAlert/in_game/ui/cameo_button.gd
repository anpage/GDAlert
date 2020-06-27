class_name CameoButton
extends Button


signal cameo_construction_started(buildable_type, buildable_id)
signal cameo_construction_held(buildable_type, buildable_id)
signal cameo_construction_canceled(buildable_type, buildable_id)
signal cameo_construction_placement_started(buildable_type, buildable_id)

export var buildable_id := 0
export var buildable_type := 0
export var completed := false
export var constructing := false
export var on_hold := false


func _gui_input(event):
	if event is InputEventMouseButton:
		if !event.pressed:
			if event.button_index == BUTTON_LEFT:
				if completed:
					emit_signal("cameo_construction_placement_started", buildable_type, buildable_id)
				elif on_hold or not constructing:
					emit_signal("cameo_construction_started", buildable_type, buildable_id)
			if event.button_index == BUTTON_RIGHT:
				if not on_hold and constructing:
					emit_signal("cameo_construction_held", buildable_type, buildable_id)
				else:
					emit_signal("cameo_construction_canceled", buildable_type, buildable_id)

