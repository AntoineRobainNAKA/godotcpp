// Created by vidal on 10/12/2024.
#pragma once

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>
#include <limits>

inline std::vector<std::vector<float>> q_learning(
        ModelFreeEnv& env,
        std::size_t num_episodes,
        float learning_rate,
        float gamma,
        float epsilon
) {
    std::size_t num_states = env.num_states();
    std::size_t num_actions = env.num_actions();

    std::vector<std::vector<float>> q_values(
            num_states,
            std::vector<float>(num_actions, 0.0f)
    );

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (std::size_t episode = 0; episode < num_episodes; ++episode) {
        env.reset();
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
            std::size_t s_p = env.state_id();

            std::vector<std::size_t> available_actions_s_p = env.available_actions();

            float q_s_p = 0.0f;
            if (!available_actions_s_p.empty()) {
                q_s_p = -std::numeric_limits<float>::infinity();
                for (std::size_t action : available_actions_s_p) {
                    float q = q_values[s_p][action];
                    if (q > q_s_p) {
                        q_s_p = q;
                    }
                }
            }

            q_values[s][a] += learning_rate * (r + gamma * q_s_p - q_values[s][a]);
        }
    }

    return q_values;
}