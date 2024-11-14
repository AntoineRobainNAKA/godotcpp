extends GridWorldNode

var calculationTime: float = 0.0
var calculationComplete: bool = true;

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if (Input.is_action_just_pressed("ui_accept")):
		print("Policy Iteration on GridWorld")
		launch_policy_iteration(0, 0)
		calculationComplete = false
	
	if (Input.is_action_just_pressed("ui_cancel")):
		print("Q-Learning on GridWorld")
		launch_q_learning()
		calculationComplete = false
		
	if (Input.is_key_label_pressed(KEY_A)):
		print("Value Iteration on GridWorld")
		launch_value_iteration()
		calculationComplete = false
		
		
	if is_calculation_complete():
		var result = get_result()
		print(result)
		calculationComplete = true
		print("Took " + str(calculationTime) + " seconds")
		calculationTime = 0.0
		
		
	if !calculationComplete:
		calculationTime += delta
