#pragma once
#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_ONPOLICYFIRSTVISITMONTECARLO_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_ONPOLICYFIRSTVISITMONTECARLO_H

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>
#include <limits>
#include "../contracts/ModelFreeEnv.h" // Include the ModelFreeEnv interface
inline std::pair<std::vector<std::vector<float>>, std::vector<std::vector<float>>>
on_policy_first_visit_monte_carlo_control(
        ModelFreeEnv& env,
        std::size_t num_episodes,
        float gamma,
        float epsilon
) {
    std::size_t num_states = env.num_states();
    std::size_t num_actions = env.num_actions();

    std::vector<std::vector<float>> q_values(
            num_states,
            std::vector<float>(num_actions, 0.0f)
    );

    std::vector<std::vector<float>> returns_sum(
            num_states,
            std::vector<float>(num_actions, 0.0f)
    );

    std::vector<std::vector<std::size_t>> returns_count(
            num_states,
            std::vector<std::size_t>(num_actions, 0)
    );

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (std::size_t episode = 0; episode < num_episodes; ++episode) {
        env.reset();
        std::vector<std::tuple<std::size_t, std::size_t, float>> episode_history;
        float episode_return = 0.0f;

        while (!env.is_game_over()) {
            std::size_t s = env.state_id();
            std::vector<std::size_t> available_actions = env.available_actions();

            float rand_num = dist(rng);
            std::size_t a;

            if (rand_num < epsilon) {
                std::uniform_int_distribution<std::size_t> action_dist(0, available_actions.size() - 1);
                a = available_actions[action_dist(rng)];
            } else {
                float max_q = -std::numeric_limits<float>::infinity();
                std::size_t best_action = available_actions[0];
                for (std::size_t action : available_actions) {
                    float q = q_values[s][action];
                    if (q > max_q) {
                        max_q = q;
                        best_action = action;
                    }
                }
                a = best_action;
            }

            float previous_score = env.score();
            env.step(a);
            float r = env.score() - previous_score;
            episode_return += r;

            episode_history.emplace_back(s, a, r);
        }

        std::vector<bool> visited_state_action_pairs(num_states * num_actions, false);

        float G = 0.0f;
        for (auto it = episode_history.rbegin(); it != episode_history.rend(); ++it) {
            auto [s, a, r] = *it;
            G = gamma * G + r;

            std::size_t state_action_index = s * num_actions + a;
            if (!visited_state_action_pairs[state_action_index]) {
                visited_state_action_pairs[state_action_index] = true;
                returns_sum[s][a] += G;
                returns_count[s][a]++;
                q_values[s][a] = returns_sum[s][a] / returns_count[s][a];
            }
        }
    }

    return { q_values, returns_sum };
}

#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_ONPOLICYFIRSTVISITMONTECARLO_H 