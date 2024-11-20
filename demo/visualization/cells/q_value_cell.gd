extends GridCell

func refresh_display() -> void:
	var container = get_child(0) as VBoxContainer
	
	# Clear existing displays
	for child in container.get_children():
		child.queue_free()
	
	for i in range(0, 4, 1):
		var q_label = Label.new()
		q_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		
		var direction = ""
		match i:
			0: direction = "↑"
			1: direction = "↓"
			2: direction = "←"
			3: direction = "→"
			
		q_label.text = "Q(%s): %.2f" % [direction, cell_data["q"][i]]
		container.add_child(q_label)
		
	# Display value if it exists
	if "value" in cell_data:
		var value_label = Label.new()
		value_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		value_label.text = "V: %.2f" % cell_data["value"]
		container.add_child(value_label)
