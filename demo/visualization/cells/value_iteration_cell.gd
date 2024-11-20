extends GridCell

func refresh_display() -> void:
	var container = get_child(0) as VBoxContainer
	
	# Clear existing displays
	for child in container.get_children():
		child.queue_free()
	
	if "value" in cell_data:
		var value_label = Label.new()
		value_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		value_label.text = "V: %.2f" % cell_data["value"]
		container.add_child(value_label)
