class_name CameoButton
extends Button


signal cameo_construction_started(buildable_type, buildable_id)
signal cameo_construction_held(buildable_type, buildable_id)
signal cameo_construction_canceled(buildable_type, buildable_id)
signal cameo_construction_placement_started(buildable_type, buildable_id)

export var buildable_id := 0
export var buildable_type := 0
export var completed := false setget _set_completed
export var constructing := false
export var on_hold := false setget _set_on_hold
export var progress := 0.0 setget _set_progress


func _ready():
	size_flags_horizontal = SIZE_EXPAND_FILL
	rect_min_size = Vector2(0, 100)
	focus_mode = Control.FOCUS_NONE


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


func _set_progress(value):
	progress = value
	$ProgressBar.value = value

	if value > 0.0:
		$ProgressBar.visible = true
	else:
		$ProgressBar.visible = false


func _set_completed(value):
	completed = value
	$CompletedOverlay.visible = value


func _set_on_hold(value):
	on_hold = value
	$HeldOverlay.visible = value
