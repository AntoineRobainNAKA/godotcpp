class_name Visualization
extends Control

@export var environment_container: EnvironmentsContainer

@onready var environment_dropdown: OptionButton = %EnvironmentDropdown
@onready var algorithm_dropdown: OptionButton = %AlgorithmDropdown
@onready var grid_visualizer: GridVisualizer = %GridVisualizer
@onready var input1: SpinBox = %Input1
@onready var input2: SpinBox = %Input2


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for environment in Globals.EnvironmentType:
		environment_dropdown.add_item(environment)
		
	for algorithm in Globals.AlgorithmType:
		algorithm_dropdown.add_item(algorithm)
		
func show_result(algorithm, result: String, grid_size: Vector2i, num_actions: int):
	grid_visualizer.update_visualization(algorithm, result, grid_size, num_actions)

func _on_start_button_pressed() -> void:
	print("launching..!")
	environment_container.launch(
		environment_dropdown.selected,
		algorithm_dropdown.selected, 
		int(input1.value), 
		int(input2.value))
