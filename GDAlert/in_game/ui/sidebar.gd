extends Panel


var money := 0

onready var money_label: Label = $SidebarContainer/Header/MarginContainer/Money


func _ready():
	money_label.text = str(int(money))


func _on_sidebar_state_updated(state):
	money = state["credits_counter"]
	money_label.text = str(int(money))
