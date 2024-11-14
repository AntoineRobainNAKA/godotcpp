extends MarginContainer

# Configurable parameters
@export var cell_size: Vector2 = Vector2(64, 64)

@export var cell_scene: PackedScene

var grid_container: GridContainer
var environment_size: Vector2i = Vector2i(5, 5)  # Default 5x5, can be changed

func _ready() -> void:    
	# Create grid container
	grid_container = GridContainer.new()
	add_child(grid_container)
	
	# Configure grid
	grid_container.columns = environment_size.x
	
	# Generate grid cells
	generate_grid()

func generate_grid() -> void:
	# Clear existing cells if any
	for child in grid_container.get_children():
		child.queue_free()
	
	# Calculate total cells needed
	var total_cells = environment_size.x * environment_size.y
	
	# Create cells
	for i in range(total_cells):
		var cell = create_grid_cell(i)
		grid_container.add_child(cell)

func create_grid_cell(index: int) -> Control:
	var cell = cell_scene.instantiate()
	cell.cell_index = index
	cell.cell_size = cell_size
	
	# Connect cell signals if needed
	#cell.cell_clicked.connect(_on_cell_clicked)
	
	return cell

# Call this when you get new results from your algorithm
func update_grid_size(rows: int, cols: int) -> void:
	environment_size = Vector2i(cols, rows)
	grid_container.columns = cols
	generate_grid()
