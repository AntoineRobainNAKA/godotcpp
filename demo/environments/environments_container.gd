extends Node

@onready var lineworldnode: LineWorldNode = $LineWorldNode
@onready var gridworldnode: GridWorldNode = $GridWorldNode

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func launch(environment: Globals.EnvironmentType, algorithm: Globals.AlgorithmType, 
		size_x: int, size_y: int = 0) -> void:
	var node
	
	match environment:
		Globals.EnvironmentType.NONE:
			return
		Globals.EnvironmentType.LINEWORLD:
			node = lineworldnode
		Globals.EnvironmentType.GRIDWORLD:
			node = gridworldnode
