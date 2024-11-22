class_name EnvironmentsContainer
extends Node

@export var visualization: Visualization

@onready var lineworldnode: LineWorldNode = $LineWorldNode
@onready var gridworldnode: GridWorldNode = $GridWorldNode

var calculationTime: float = 0.0
var calculationComplete: bool = true

var current_node
var algorithm_type: Globals.AlgorithmType
var grid_size: Vector2i

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
		visualization.show_result(algorithm_type, result, grid_size)
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
			lineworldnode.launch_algorithm(algorithm, size_x)
			print("launching on lineworld")
		Globals.EnvironmentType.GRIDWORLD:
			current_node = gridworldnode
			gridworldnode.launch_algorithm(algorithm, size_x, size_y)
			print("launching on gridworld")
