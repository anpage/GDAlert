extends Control

func _process(delta):
	if delta != 0:
		$FramerateLabel.text = "FPS: " + str((1/delta))
