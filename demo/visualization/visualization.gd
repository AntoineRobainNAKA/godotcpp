extends Control

@onready var environment_dropdown: OptionButton = %EnvironmentDropdown
@onready var algorithm_dropdown: OptionButton = %AlgorithmDropdown
@onready var grid_visualizer: GridVisualizer = %GridVisualizer

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for environment in Globals.Environments:
		environment_dropdown.add_item(environment)
		
	for algorithm in Globals.AlgorithmType:
		algorithm_dropdown.add_item(algorithm)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_start_button_pressed() -> void:
	pass # Replace with function body.
