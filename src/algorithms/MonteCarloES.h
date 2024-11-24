#pragma once

#include <vector>
#include <cstddef>
#include <random>
#include <algorithm>
#include <limits>
#include "../contracts/ModelFreeEnv.h" // Include the ModelFreeEnv interface

template<typename TEnv>
std::vector<std::vector<float>> monte_carlo_es(
    TEnv& env,
    std::size_t num_episodes,
    float epsilon
) {
    std::size_t num_states = env.num_states();
    std::size_t num_actions = env.num_actions();

    std::vector<std::vector<float>> q_values(
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
        std::vector<std::pair<std::size_t, std::size_t>> episode_history;
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

            episode_history.emplace_back(s, a);
        }

        for (const auto& [s, a] : episode_history) {
            returns_count[s][a]++;
            q_values[s][a] += (episode_return - q_values[s][a]) / returns_count[s][a];
        }
    }

    return q_values;
}
