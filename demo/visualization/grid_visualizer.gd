class_name GridVisualizer
extends MarginContainer

# Configurable parameters
@export var cell_size: Vector2 = Vector2(64, 64)

@export var policy_iteration_cell: PackedScene
@export var q_learning_cell: PackedScene
@export var value_iteration_cell: PackedScene

var grid_container: GridContainer
var environment_size: Vector2i = Vector2i(5, 5)  # Default 5x5, can be changed
var current_algorithm: Globals.AlgorithmType

func _ready() -> void:    
	setup_grid()
	
func setup_grid() -> void:
	if grid_container == null:
		grid_container = GridContainer.new()
		add_child(grid_container)
	grid_container.columns = environment_size.x

func generate_grid(algorithm: Globals.AlgorithmType, grid_size: Vector2i) -> void:
	current_algorithm = algorithm

	var cell_scene = get_cell_scene_for_algorithm(algorithm)
	if cell_scene == null:
		push_error("No cell scene set for algorithm type: " + str(algorithm))
		return

	# Clear existing cells
	for child in grid_container.get_children():
		child.queue_free()

	if grid_size != environment_size:
		environment_size = grid_size
		setup_grid()
	
	var total_cells = environment_size.x * environment_size.y

	for i in range(total_cells):
			var cell: GridCell = cell_scene.instantiate()
			cell.cell_index = i
			cell.cell_size = cell_size
			cell.cell_clicked.connect(_on_cell_clicked)
			grid_container.add_child(cell)

func get_cell_scene_for_algorithm(algorithm: Globals.AlgorithmType) -> PackedScene:
	match algorithm:
		Globals.AlgorithmType.POLICY_ITERATION:
			return policy_iteration_cell
		Globals.AlgorithmType.VALUE_ITERATION:
			return value_iteration_cell
		Globals.AlgorithmType.Q_LEARNING:
			return q_learning_cell
		Globals.AlgorithmType.MONTE_CARLO_ES:
			return q_learning_cell
		Globals.AlgorithmType.ON_POLICY_FIRST_VISIT_MC:
			return q_learning_cell
		Globals.AlgorithmType.DYNA_Q:
			return q_learning_cell
		_:
			return null
			
func update_visualization(algorithm: Globals.AlgorithmType, result: String, grid_size: Vector2i, num_actions: int) -> void:
	print("Updating visualization")
	
	# If grid isn't generated or algorithm changed, generate new grid
	if grid_container.get_child_count() == 0 or current_algorithm != algorithm or grid_size != environment_size:
		print("Generating new grid")
		generate_grid(algorithm, grid_size)
	
	match algorithm:
		Globals.AlgorithmType.POLICY_ITERATION:
			parse_policy_iteration_results(result)
		Globals.AlgorithmType.VALUE_ITERATION:
			parse_value_iteration_results(result)
		Globals.AlgorithmType.Q_LEARNING:
			parse_q_learning_results(result, num_actions)
		Globals.AlgorithmType.MONTE_CARLO_ES:
			parse_q_learning_results(result, num_actions)
		Globals.AlgorithmType.ON_POLICY_FIRST_VISIT_MC:
			parse_q_learning_results(result, num_actions)
		_:
			print("Didn't match any algorithm !")

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
	var values = []
	
	for line in result_text.split("\n"):
		if line.begins_with("V"):
			var value = float(line.split("=")[2])
			values.append(value)
	
	update_value_iteration_data(values)

func update_value_iteration_data(values: Array) -> void:
	var cells = grid_container.get_children()
	for i in range(cells.size()):
		if i < values.size():
			cells[i].update_data("value", values[i])

func parse_q_learning_results(result_text: String, num_actions: int) -> void:
	var qs = []
	var cur_num_actions = 0
	var state_q = []
	
	for line in result_text.split("\n"):
		if line.begins_with("Q"):
			var value = float(line.split("=")[3])
			state_q.append(value)
			cur_num_actions += 1
			
			if cur_num_actions == num_actions:
				qs.append(state_q)
				state_q = [] 
				cur_num_actions = 0
	
	update_q_learning_data(qs, num_actions)

func update_q_learning_data(qs: Array, num_actions: int) -> void:
	print("Updating q learning data...")
	var cells = grid_container.get_children()
	for i in range(cells.size()):
		if i < qs.size():
			var cell: GridCell = cells[i]
			cell.num_actions = num_actions
			cell.update_data("q", qs[i])

func _on_cell_clicked(cell_index: int) -> void:
	print("Cell clicked: ", cell_index)
