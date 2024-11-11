//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENVWRAPPER_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENVWRAPPER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "DynamicLibrary.h"

class SecretEnvWrapper {
public:
    using NumStatesFn = std::size_t (*)();
    using NumActionsFn = std::size_t (*)();
    using NumRewardsFn = std::size_t (*)();
    using RewardFn = float (*)(std::size_t);
    using TransitionProbabilityFn = float (*)(std::size_t, std::size_t, std::size_t, std::size_t);

    using NewFn = void* (*)();
    using FromRandomStateFn = void* (*)();

    using ResetFn = void (*)(void*);
    using StateIdFn = std::size_t (*)(void*);
    using IsForbiddenFn = bool (*)(void*, std::size_t);
    using IsGameOverFn = bool (*)(void*);
    using AvailableActionsFn = std::size_t* (*)(void*);
    using AvailableActionsLenFn = std::size_t (*)(void*);
    using AvailableActionsDeleteFn = void (*)(std::size_t*, std::size_t);

    using StepFn = void (*)(void*, std::size_t);
    using ScoreFn = float (*)(void*);

    using DeleteFn = void (*)(void*);

    explicit SecretEnvWrapper(uint8_t secret_env_id);
    ~SecretEnvWrapper() = default;

    NumStatesFn num_states_fn;
    NumActionsFn num_actions_fn;
    NumRewardsFn num_rewards_fn;
    RewardFn reward_fn;
    TransitionProbabilityFn transition_probability_fn;

    NewFn new_fn;
    FromRandomStateFn from_random_state_fn;

    ResetFn reset_fn;
    StateIdFn state_id_fn;
    IsForbiddenFn is_forbidden_fn;
    IsGameOverFn is_game_over_fn;
    AvailableActionsFn available_actions_fn;
    AvailableActionsLenFn available_actions_len_fn;
    AvailableActionsDeleteFn available_actions_delete_fn;

    StepFn step_fn;
    ScoreFn score_fn;

    DeleteFn delete_fn;

private:
    DynamicLibrary lib;

    template<typename T>
    T load_function(const std::string& func_name);
};

#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENVWRAPPER_H
