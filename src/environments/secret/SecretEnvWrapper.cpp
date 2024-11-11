//
// Created by vidal on 10/12/2024.
//

#include "Paths.h"
#include "SecretEnvWrapper.h"

SecretEnvWrapper::SecretEnvWrapper(uint8_t secret_env_id) : lib(SECRET_ENV_PATH) {
    std::string prefix = "secret_env_" + std::to_string(secret_env_id);

    num_states_fn = load_function<NumStatesFn>(prefix + "_num_states");
    num_actions_fn = load_function<NumActionsFn>(prefix + "_num_actions");
    num_rewards_fn = load_function<NumRewardsFn>(prefix + "_num_rewards");
    reward_fn = load_function<RewardFn>(prefix + "_reward");
    transition_probability_fn = load_function<TransitionProbabilityFn>(prefix + "_transition_probability");

    new_fn = load_function<NewFn>(prefix + "_new");
    from_random_state_fn = load_function<FromRandomStateFn>(prefix + "_from_random_state");

    reset_fn = load_function<ResetFn>(prefix + "_reset");
    state_id_fn = load_function<StateIdFn>(prefix + "_state_id");
    is_forbidden_fn = load_function<IsForbiddenFn>(prefix + "_is_forbidden");
    is_game_over_fn = load_function<IsGameOverFn>(prefix + "_is_game_over");
    available_actions_fn = load_function<AvailableActionsFn>(prefix + "_available_actions");
    available_actions_len_fn = load_function<AvailableActionsLenFn>(prefix + "_available_actions_len");
    available_actions_delete_fn = load_function<AvailableActionsDeleteFn>(prefix + "_available_actions_delete");

    step_fn = load_function<StepFn>(prefix + "_step");
    score_fn = load_function<ScoreFn>(prefix + "_score");

    delete_fn = load_function<DeleteFn>(prefix + "_delete");
}

template<typename T>
T SecretEnvWrapper::load_function(const std::string& func_name) {
    void* func = lib.loadSymbol(func_name);
    return reinterpret_cast<T>(func);
}

