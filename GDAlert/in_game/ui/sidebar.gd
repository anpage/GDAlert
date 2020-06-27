extends Panel


signal cameo_construction_started(buildable_type, buildable_id)
signal cameo_construction_held(buildable_type, buildable_id)
signal cameo_construction_canceled(buildable_type, buildable_id)
signal cameo_construction_placement_started(buildable_type, buildable_id)

var money := 0

onready var CameoButtonScene = preload("res://in_game/ui/cameo_button.tscn")

onready var money_label: Label = $SidebarContainer/Header/MarginContainer/Money


func _ready():
	money_label.text = str(int(money))


func _on_sidebar_state_updated(state):
	money = state["credits_counter"]
	money_label.text = str(int(money))

	var production_cameos := $SidebarContainer/PowerAndProductionMargin/PowerAndProduction/Production/ProductionScroll/ProductionCameos
	var cameos := production_cameos.get_children()
	var removed_cameos := production_cameos.get_children()

	for entry in state["entries"]:
		var id: int = entry["buildable_id"]

		var current_cameo: CameoButton
		for cameo in cameos:
			if cameo.buildable_id == id:
				current_cameo = cameo as CameoButton
				break

		if current_cameo != null:
			removed_cameos.remove(removed_cameos.find(current_cameo))

			# Cameo stuff updates
			current_cameo.completed = entry["completed"]
			current_cameo.constructing = entry["constructing"]
			current_cameo.on_hold = entry["construction_on_hold"]
			current_cameo.progress = entry["progress"]

		else:
			var cameo = CameoButtonScene.instance() as CameoButton

			# Button stuff
			cameo.text = entry["asset_name"]

			# Cameo stuff
			cameo.buildable_id = id
			cameo.buildable_type = entry["buildable_type"]
			cameo.completed = entry["completed"]
			cameo.constructing = entry["constructing"]
			cameo.on_hold = entry["construction_on_hold"]
			cameo.progress = entry["progress"]

			# Signals
			var _connected = cameo.connect("cameo_construction_started", self, "_on_cameo_construction_started")
			_connected = cameo.connect("cameo_construction_held", self, "_on_cameo_construction_held")
			_connected = cameo.connect("cameo_construction_canceled", self, "_on_cameo_construction_canceled")
			_connected = cameo.connect("cameo_construction_placement_started", self, "_on_cameo_construction_placement_started")

			production_cameos.add_child(cameo)

	for cameo in removed_cameos:
		cameo.queue_free()


func _on_cameo_construction_started(buildable_type, buildable_id):
	emit_signal("cameo_construction_started", buildable_type, buildable_id)


func _on_cameo_construction_held(buildable_type, buildable_id):
	emit_signal("cameo_construction_held", buildable_type, buildable_id)


func _on_cameo_construction_canceled(buildable_type, buildable_id):
	emit_signal("cameo_construction_canceled", buildable_type, buildable_id)


func _on_cameo_construction_placement_started(buildable_type, buildable_id):
	emit_signal("cameo_construction_placement_started", buildable_type, buildable_id)
