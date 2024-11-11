//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENV_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENV_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include "SecretEnvWrapper.h"
#include "../../contracts/MDPEnv.h"
#include "../../contracts/ModelFreeEnv.h"

template<uint8_t ENV_ID>
class SecretEnv : public MDPEnv, public ModelFreeEnv {
private:
    void* instance;

    static SecretEnvWrapper& get_wrapper();

    explicit SecretEnv(void* instance);

public:
    SecretEnv();

    ~SecretEnv() override;

    SecretEnv(const SecretEnv&) = delete;
    SecretEnv& operator=(const SecretEnv&) = delete;

    [[nodiscard]] std::size_t num_states() const override;
    [[nodiscard]] std::size_t num_actions() const override;
    [[nodiscard]] std::size_t num_rewards() const override;
    [[nodiscard]] float reward(std::size_t index) const override;
    [[nodiscard]] float transition_probability(std::size_t state, std::size_t action,
                                 std::size_t next_state, std::size_t reward_index) const override;

    [[maybe_unused]] static SecretEnv<ENV_ID> from_random_state();
    void reset() override;
    [[nodiscard]] bool is_game_over() const override;
    [[nodiscard]] float score() const override;
    [[nodiscard]] std::size_t state_id() const override;
    [[nodiscard]] bool is_forbidden(std::size_t action) const override;
    [[nodiscard]] std::vector<std::size_t> available_actions() const override;
    void step(std::size_t action) override;
};

template<uint8_t ENV_ID>
SecretEnvWrapper& SecretEnv<ENV_ID>::get_wrapper() {
    static SecretEnvWrapper wrapper(ENV_ID);
    return wrapper;
}

template<uint8_t ENV_ID>
SecretEnv<ENV_ID>::SecretEnv() : instance(nullptr) {
    instance = get_wrapper().new_fn();
    if (!instance) {
        throw std::runtime_error("Failed to create instance");
    }
}

template<uint8_t ENV_ID>
SecretEnv<ENV_ID>::SecretEnv(void* instance) : instance(instance) {
    if (!instance) {
        throw std::runtime_error("Instance is null");
    }
}

template<uint8_t ENV_ID>
SecretEnv<ENV_ID>::~SecretEnv() {
    if (instance) {
        get_wrapper().delete_fn(instance);
        instance = nullptr;
    }
}

template<uint8_t ENV_ID>
[[maybe_unused]] SecretEnv<ENV_ID> SecretEnv<ENV_ID>::from_random_state() {
    void* instance = get_wrapper().from_random_state_fn();
    if (!instance) {
        throw std::runtime_error("Failed to create instance from random state");
    }
    return SecretEnv<ENV_ID>(instance);
}

template<uint8_t ENV_ID>
std::size_t SecretEnv<ENV_ID>::num_states() const {
    return get_wrapper().num_states_fn();
}

template<uint8_t ENV_ID>
std::size_t SecretEnv<ENV_ID>::num_actions() const {
    return get_wrapper().num_actions_fn();
}

template<uint8_t ENV_ID>
std::size_t SecretEnv<ENV_ID>::num_rewards() const {
    return get_wrapper().num_rewards_fn();
}

template<uint8_t ENV_ID>
float SecretEnv<ENV_ID>::reward(std::size_t index) const {
    return get_wrapper().reward_fn(index);
}

template<uint8_t ENV_ID>
float SecretEnv<ENV_ID>::transition_probability(std::size_t state, std::size_t action,
                                                std::size_t next_state, std::size_t reward_index) const {
    return get_wrapper().transition_probability_fn(state, action, next_state, reward_index);
}

template<uint8_t ENV_ID>
void SecretEnv<ENV_ID>::reset() {
    get_wrapper().reset_fn(instance);
}

template<uint8_t ENV_ID>
bool SecretEnv<ENV_ID>::is_game_over() const {
    return get_wrapper().is_game_over_fn(instance);
}

template<uint8_t ENV_ID>
float SecretEnv<ENV_ID>::score() const {
    return get_wrapper().score_fn(instance);
}

template<uint8_t ENV_ID>
std::size_t SecretEnv<ENV_ID>::state_id() const {
    return get_wrapper().state_id_fn(instance);
}

template<uint8_t ENV_ID>
bool SecretEnv<ENV_ID>::is_forbidden(std::size_t action) const {
    return get_wrapper().is_forbidden_fn(instance, action);
}

template<uint8_t ENV_ID>
std::vector<std::size_t> SecretEnv<ENV_ID>::available_actions() const {
    std::vector<std::size_t> actions;
    std::size_t* raw_actions = get_wrapper().available_actions_fn(instance);
    std::size_t actions_len = get_wrapper().available_actions_len_fn(instance);

    for (std::size_t i = 0; i < actions_len; ++i) {
        actions.push_back(raw_actions[i]);
    }

    // Libération de la mémoire allouée par la bibliothèque
    get_wrapper().available_actions_delete_fn(raw_actions, actions_len);

    return actions;
}

template<uint8_t ENV_ID>
void SecretEnv<ENV_ID>::step(std::size_t action) {
    get_wrapper().step_fn(instance, action);
}

using SecretEnv0 [[maybe_unused]] = SecretEnv<0>;
using SecretEnv1 [[maybe_unused]] = SecretEnv<1>;
using SecretEnv2 [[maybe_unused]] = SecretEnv<2>;
using SecretEnv3 [[maybe_unused]] = SecretEnv<3>;

#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_SECRETENV_H
