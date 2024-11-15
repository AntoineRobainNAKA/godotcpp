class_name GridCell
extends PanelContainer

# base class for grid cells

signal cell_clicked(cell_index: int)

# Basic cell properties
@export var cell_index: int = 0
@export var cell_size: Vector2 = Vector2(64, 64)

# Dictionary to store different types of values
var cell_data: Dictionary = {}

func _ready() -> void:
	custom_minimum_size = cell_size
	
	# Setup basic appearance
	setup_base_layout()
	
	# Connect signals
	gui_input.connect(_on_gui_input)

func setup_base_layout() -> void:
	# Create a VBoxContainer to stack different value displays
	var container = VBoxContainer.new()
	container.alignment = BoxContainer.ALIGNMENT_CENTER
	add_child(container)

# Generic method to update any type of data
func update_data(data_type: String, value) -> void:
	cell_data[data_type] = value
	refresh_display()

# Clear specific or all data
func clear_data(data_type: String = "") -> void:
	if data_type.is_empty():
		cell_data.clear()
	else:
		cell_data.erase(data_type)
	refresh_display()

# Virtual method - override in specialized cells
func refresh_display() -> void:
	# Basic implementation - can be overridden
	var container = get_child(0) as VBoxContainer
	
	# Clear existing displays
	for child in container.get_children():
		child.queue_free()
	
	# Create labels for each piece of data
	for data_type in cell_data:
		var label = Label.new()
		label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		label.text = "%s: %s" % [data_type, str(cell_data[data_type])]
		container.add_child(label)

func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.pressed:
		cell_clicked.emit(cell_index)
