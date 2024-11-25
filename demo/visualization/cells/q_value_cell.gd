extends GridCell

var num_actions: int

func refresh_display() -> void:
	var container = get_child(0) as VBoxContainer
	
	# Clear existing displays
	for child in container.get_children():
		child.queue_free()
	
	for i in range(0, num_actions, 1):
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
