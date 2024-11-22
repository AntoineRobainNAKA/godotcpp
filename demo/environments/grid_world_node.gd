extends GridWorldNode

@onready var grid_vis: GridVisualizer = $"../GridVisualizer"

var calculationTime: float = 0.0
var calculationComplete: bool = true
var algorithm_type: Globals.AlgorithmType = Globals.AlgorithmType.NONE

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if (Input.is_action_just_pressed("ui_accept")):
		print("Policy Iteration on GridWorld")		
		algorithm_type = Globals.AlgorithmType.POLICY_ITERATION
		launch_algorithm(algorithm_type, 5, 5)
		calculationComplete = false
	
	if (Input.is_action_just_pressed("ui_cancel")):
		print("Q-Learning on GridWorld")
		#launch_q_learning(5, 5)
		algorithm_type = Globals.AlgorithmType.Q_LEARNING
		calculationComplete = false
		
	if (Input.is_key_label_pressed(KEY_A)):
		print("Value Iteration on GridWorld")
		#launch_value_iteration(5, 5)
		algorithm_type = Globals.AlgorithmType.VALUE_ITERATION
		calculationComplete = false
		
		
	if is_calculation_complete():
		var result = get_result()
		print(result)
		calculationComplete = true
		print("Took " + str(calculationTime) + " seconds")
		calculationTime = 0.0
		#grid_vis.update_visualization(algorithm_type, result, Vector2i(10, 10))
		algorithm_type = Globals.AlgorithmType.NONE
				
	if !calculationComplete:
		calculationTime += delta
