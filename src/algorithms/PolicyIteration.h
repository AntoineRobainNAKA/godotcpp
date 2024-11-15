//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_POLICYITERATION_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_POLICYITERATION_H


#include <vector>
#include <utility>
#include <cmath>
#include <limits>

template<typename TEnv>
std::pair<std::vector<std::size_t>, std::vector<float>>
policy_iteration(const TEnv& env, float gamma, float theta) {
    std::size_t num_states = env.num_states();
    std::size_t num_actions = env.num_actions();
    std::size_t num_rewards = env.num_rewards();

    std::vector<std::size_t> pi(num_states, 0);
    std::vector<float> value_function(num_states, 0.0f);

    std::vector<float> rewards(num_rewards);
    for (std::size_t r_index = 0; r_index < num_rewards; ++r_index) {
        rewards[r_index] = env.reward(r_index);
    }

    std::size_t num_floats = num_states * num_actions * num_states * num_rewards;
    std::vector<float> cached_transition_probabilities(num_floats, 42.0f);

    for (std::size_t s = 0; s < num_states; ++s) {
        std::size_t s_index = s * num_actions * num_states * num_rewards;
        for (std::size_t a = 0; a < num_actions; ++a) {
            std::size_t a_index = s_index + a * num_states * num_rewards;
            for (std::size_t s_p = 0; s_p < num_states; ++s_p) {
                std::size_t s_p_index = a_index + s_p * num_rewards;
                for (std::size_t r_index = 0; r_index < num_rewards; ++r_index) {
                    std::size_t index = s_p_index + r_index;
                    cached_transition_probabilities[index] = env.transition_probability(s, a, s_p, r_index);
                }
            }
        }
    }

    while (true) {
        // Policy Evaluation
        while (true) {
            float delta = 0.0f;
            for (std::size_t s = 0; s < num_states; ++s) {
                float v = value_function[s];
                std::size_t a = pi[s];
                float total = 0.0f;
                std::size_t s_index = s * num_actions * num_states * num_rewards;
                std::size_t a_index = s_index + a * num_states * num_rewards;
                for (std::size_t s_p = 0; s_p < num_states; ++s_p) {
                    std::size_t s_p_index = a_index + s_p * num_rewards;
                    for (std::size_t r_index = 0; r_index < num_rewards; ++r_index) {
                        std::size_t index = s_p_index + r_index;
                        total += cached_transition_probabilities[index] *
                                 (rewards[r_index] + gamma * value_function[s_p]);
                    }
                }
                value_function[s] = total;
                delta = std::max(delta, std::fabs(v - value_function[s]));
            }
            if (delta < theta) {
                break;
            }
        }

        // Policy Improvement
        bool policy_stable = true;
        for (std::size_t s = 0; s < num_states; ++s) {
            std::size_t old_action = pi[s];
            std::size_t s_index = s * num_actions * num_states * num_rewards;

            std::size_t best_action = 0;
            float best_value = -std::numeric_limits<float>::infinity();

            for (std::size_t a = 0; a < num_actions; ++a) {
                float total = 0.0f;
                std::size_t a_index = s_index + a * num_states * num_rewards;
                for (std::size_t s_p = 0; s_p < num_states; ++s_p) {
                    std::size_t s_p_index = a_index + s_p * num_rewards;
                    for (std::size_t r_index = 0; r_index < num_rewards; ++r_index) {
                        std::size_t index = s_p_index + r_index;
                        total += cached_transition_probabilities[index] *
                                 (rewards[r_index] + gamma * value_function[s_p]);
                    }
                }

                if (total > best_value) {
                    best_value = total;
                    best_action = a;
                }
            }

            pi[s] = best_action;

            if (old_action != pi[s]) {
                policy_stable = false;
            }
        }

        if (policy_stable) {
            break;
        }
    }

    return { pi, value_function };
}

#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_POLICYITERATION_H
