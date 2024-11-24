class_name EnvironmentsContainer
extends Node

@export var visualization: Visualization

@onready var lineworldnode: LineWorldNode = $LineWorldNode
@onready var gridworldnode: GridWorldNode = $GridWorldNode
@onready var rpsnode: RPSNode = $RPSNode
@onready var secretenv0node: SecretEnv0Node = $SecretEnv0Node

var calculationTime: float = 0.0
var calculationComplete: bool = true

var current_node
var algorithm_type: Globals.AlgorithmType
var grid_size: Vector2i
var num_actions: int

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if !calculationComplete:
		calculationTime += delta
		
	if current_node != null and current_node.is_calculation_complete():
		var result = current_node.get_result()
		print(result)
		calculationComplete = true		
		print("Took " + str(calculationTime) + " seconds")
		calculationTime = 0.0
		visualization.show_result(algorithm_type, result, grid_size, num_actions)
		algorithm_type = Globals.AlgorithmType.NONE
		grid_size = Vector2i(0, 0)
		current_node = null

func launch(environment: Globals.EnvironmentType, algorithm: Globals.AlgorithmType, 
		size_x: int, size_y: int = 1) -> void:
	if algorithm == Globals.AlgorithmType.NONE:
		return
	
	algorithm_type = algorithm
	grid_size = Vector2i(size_x, size_y)
	calculationComplete = false
	
	match environment:
		Globals.EnvironmentType.NONE:
			return
		Globals.EnvironmentType.LINEWORLD:
			current_node = lineworldnode
			num_actions = 2
			lineworldnode.launch_algorithm(algorithm, size_x)
			print("launching on lineworld")
		Globals.EnvironmentType.GRIDWORLD:
			current_node = gridworldnode
			num_actions = 4
			gridworldnode.launch_algorithm(algorithm, size_x, size_y)
			print("launching on gridworld")
		Globals.EnvironmentType.RPS:
			current_node = rpsnode
			num_actions = 3
			rpsnode.launch_algorithm(algorithm)
			print("lauching on rps")
		Globals.EnvironmentType.SECRETENV0:
			current_node = secretenv0node
			secretenv0node.launch_algorithm(algorithm)
			print("launching on secret env 0")
