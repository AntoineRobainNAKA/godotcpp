extends GridWorldNode


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if (Input.is_action_just_pressed("ui_accept")):
		print("Policy Iteration on GridWorld")
		var result = launch_policy_iteration(0, 0)
		print(result)
	
	if (Input.is_action_just_pressed("ui_cancel")):
		print("Q-Learning on GridWorld")
		var result = launch_q_learning()
		print(result)
