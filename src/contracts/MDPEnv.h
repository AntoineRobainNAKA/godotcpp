// Created by vidal on 10/12/2024.
#pragma once

#include <cstddef>

class MDPEnv {
public:
    virtual ~MDPEnv() = default;

    [[maybe_unused]] [[nodiscard]] virtual std::size_t num_states() const = 0;

    [[maybe_unused]] [[nodiscard]] virtual std::size_t num_actions() const = 0;

    [[maybe_unused]] [[nodiscard]] virtual std::size_t num_rewards() const = 0;

    [[maybe_unused]] [[nodiscard]] virtual float reward(std::size_t index) const = 0;

    [[maybe_unused]] [[nodiscard]] virtual float transition_probability(std::size_t state, std::size_t action,
                                         std::size_t next_state, std::size_t reward_index) const = 0;
};
