extends GridCell

func refresh_display() -> void:
	var container = get_child(0) as VBoxContainer
	
	# Clear existing displays
	for child in container.get_children():
		child.queue_free()
	
	# Display value if it exists
	if "value" in cell_data:
		var value_label = Label.new()
		value_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		value_label.text = "V: %.2f" % cell_data["value"]
		container.add_child(value_label)
	
	# Display policy if it exists
	if "policy" in cell_data:
		var policy_label = Label.new()
		policy_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		
		# Convert policy number to direction
		var direction = ""
		match cell_data["policy"]:
			0: direction = "↑"
			1: direction = "↓"
			2: direction = "←"
			3: direction = "→"
		
		policy_label.text = "π: %s" % direction
		container.add_child(policy_label)
