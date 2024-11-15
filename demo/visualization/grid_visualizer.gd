class_name GridVisualizer
extends MarginContainer

# Configurable parameters
@export var cell_size: Vector2 = Vector2(64, 64)

@export var policy_iteration_cell: PackedScene
@export var q_learning_cell: PackedScene
@export var value_iteration_cell: PackedScene

enum AlgorithmType {
	NONE,
	POLICY_ITERATION,
	VALUE_ITERATION,
	Q_LEARNING
}

var grid_container: GridContainer
var environment_size: Vector2i = Vector2i(5, 5)  # Default 5x5, can be changed
var current_algorithm: AlgorithmType

func _ready() -> void:    
	setup_grid()
	
func setup_grid() -> void:
	grid_container = GridContainer.new()
	add_child(grid_container)
	grid_container.columns = environment_size.x

func generate_grid(algorithm: AlgorithmType) -> void:
	current_algorithm = algorithm

	# Clear existing cells
	for child in grid_container.get_children():
		child.queue_free()
	
	var cell_scene = get_cell_scene_for_algorithm(algorithm)
	if cell_scene == null:
		push_error("No cell scene set for algorithm type: " + str(algorithm))
		return
	
	var total_cells = environment_size.x * environment_size.y

	for i in range(total_cells):
			var cell: GridCell = cell_scene.instantiate()
			cell.cell_index = i
			cell.cell_size = cell_size
			cell.cell_clicked.connect(_on_cell_clicked)
			grid_container.add_child(cell)

func get_cell_scene_for_algorithm(algorithm: AlgorithmType) -> PackedScene:
	match algorithm:
		AlgorithmType.POLICY_ITERATION:
			return policy_iteration_cell
		AlgorithmType.VALUE_ITERATION:
			return value_iteration_cell
		AlgorithmType.Q_LEARNING:
			return q_learning_cell
		_:
			return null
			
func update_visualization(algorithm: AlgorithmType, result: String) -> void:
	# If grid isn't generated or algorithm changed, generate new grid
	if grid_container.get_child_count() == 0 or current_algorithm != algorithm:
		generate_grid(algorithm)
	
	match algorithm:
		AlgorithmType.POLICY_ITERATION:
			parse_policy_iteration_results(result)
		AlgorithmType.VALUE_ITERATION:
			#parse_value_iteration_results(result)
			pass
		AlgorithmType.Q_LEARNING:
			#parse_q_learning_results(result)
			pass

# Call this when you get new results from your algorithm
func update_grid_size(rows: int, cols: int) -> void:
	environment_size = Vector2i(cols, rows)
	grid_container.columns = cols
	generate_grid(current_algorithm)

func parse_policy_iteration_results(result_text: String) -> void:
	var values = []
	var policies = []
	
	for line in result_text.split("\n"):
		if line.begins_with("Pi(s="):
			var policy = int(line.split("=")[2])
			policies.append(policy)
		elif line.begins_with("V(s="):
			var value = float(line.split("=")[2])
			values.append(value)
	
	update_policy_iteration_data(values, policies)

func update_policy_iteration_data(values: Array, policies: Array) -> void:
	var cells = grid_container.get_children()
	for i in range(cells.size()):
		if i < values.size() and i < policies.size():
			cells[i].update_data("value", values[i])
			cells[i].update_data("policy", policies[i])

func parse_value_iteration_results(result_text: String) -> void:
	pass

func update_value_iteration_data(values: Array) -> void:
	pass

func parse_q_learning_results(result_text: String) -> void:
	pass

func update_q_learning_data(data) -> void:
	pass

func _on_cell_clicked(cell_index: int) -> void:
	print("Cell clicked: ", cell_index)
