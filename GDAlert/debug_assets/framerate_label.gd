extends RichTextLabel

func _process(delta):
	if delta != 0:
		text = "FPS: " + str((1/delta))
