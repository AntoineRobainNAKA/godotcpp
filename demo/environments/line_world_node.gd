extends LineWorldNode

@onready var grid_vis: GridVisualizer = $"../GridVisualizer"

var calculationTime: float = 0.0
var calculationComplete: bool = true;
var algorithm_type: GridVisualizer.AlgorithmType = GridVisualizer.AlgorithmType.NONE

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if (Input.is_action_just_pressed("ui_up")):
		print("Policy Iteration on LineWorld")
		launch_policy_iteration(5)
		algorithm_type = GridVisualizer.AlgorithmType.POLICY_ITERATION
		calculationComplete = false
	
	if (Input.is_action_just_pressed("ui_down")):
		print("Q-Learning on LineWorld")
		launch_q_learning(5)
		algorithm_type = GridVisualizer.AlgorithmType.Q_LEARNING
		calculationComplete = false
	
	if (Input.is_action_just_pressed("ui_right")):
		print("Value Iteration on LineWorld")
		launch_value_iteration(5)
		algorithm_type = GridVisualizer.AlgorithmType.VALUE_ITERATION
		calculationComplete = false
		
	if is_calculation_complete():
		var result = get_result()
		print(result)
		calculationComplete = true
		print("Took " + str(calculationTime) + " seconds")
		calculationTime = 0.0
		grid_vis.update_visualization(algorithm_type, result, Vector2i(5, 1))
		algorithm_type = GridVisualizer.AlgorithmType.NONE		
		
	if !calculationComplete:
		calculationTime += delta
