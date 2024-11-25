#pragma once

#include <vector>
#include <deque>
#include <tuple>
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>
#include "contracts/ModelFreeEnv.h"

inline std::vector<std::vector<float>> dina_q(
        ModelFreeEnv& env,
        std::size_t num_episodes,
        float learning_rate,
        float gamma,
        float epsilon,
        std::size_t memory_capacity,
        std::size_t batch_size,
        std::size_t target_update_freq
) {
    std::size_t num_states = env.num_states();
    std::size_t num_actions = env.num_actions();

    std::vector<std::vector<float>> online_q_table(
            num_states,
            std::vector<float>(num_actions, 0.0f)
    );

    std::vector<std::vector<float>> target_q_table = online_q_table;

    std::deque<std::tuple<std::size_t, std::size_t, float, std::size_t, bool>> memory;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    std::size_t steps = 0;

    for (std::size_t episode = 0; episode < num_episodes; ++episode) {
        env.reset();
        std::size_t state = env.state_id();
        float total_reward = 0.0f;

        while (!env.is_game_over()) {
            // Epsilon-greedy action selection
            std::size_t action;
            if (dist(rng) < epsilon) {
                std::uniform_int_distribution<std::size_t> action_dist(0, num_actions - 1);
                action = action_dist(rng);
            } else {
                action = std::distance(
                        online_q_table[state].begin(),
                        std::max_element(online_q_table[state].begin(), online_q_table[state].end())
                );
            }

            // Execute action
            float previous_score = env.score();
            env.step(action);
            float reward = env.score() - previous_score;
            std::size_t next_state = env.state_id();
            bool done = env.is_game_over();

            // Store transition in memory
            if (memory.size() >= memory_capacity) {
                memory.pop_front();
            }
            memory.emplace_back(state, action, reward, next_state, done);

            // Optimize model if enough transitions are available
            if (memory.size() >= batch_size) {
                std::vector<std::tuple<std::size_t, std::size_t, float, std::size_t, bool>> batch(batch_size);
                std::sample(memory.begin(), memory.end(), batch.begin(), batch_size, rng);

                for (const auto& [s, a, r, s_p, terminal] : batch) {
                    float target = r;
                    if (!terminal) {
                        float max_q_next = *std::max_element(target_q_table[s_p].begin(), target_q_table[s_p].end());
                        target += gamma * max_q_next;
                    }
                    online_q_table[s][a] += learning_rate * (target - online_q_table[s][a]);
                }
            }

            // Update target Q-table periodically
            if (steps % target_update_freq == 0) {
                target_q_table = online_q_table;
            }

            state = next_state;
            total_reward += reward;
            steps++;
        }

        std::cout << "Episode: " << episode << ", Total Reward: " << total_reward << std::endl;
    }

    return online_q_table;
}