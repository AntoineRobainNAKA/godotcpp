// Created by vidal on 10/12/2024.
#pragma once


#include <vector>
#include <cstddef>
#include <memory>
#include <random>

class ModelFreeEnv {
public:
    virtual ~ModelFreeEnv() = default;

    static std::unique_ptr<ModelFreeEnv> create();
    static std::unique_ptr<ModelFreeEnv> from_random_state(std::mt19937& rng);

    [[maybe_unused]] [[nodiscard]] virtual std::size_t num_states() const = 0;
    [[maybe_unused]] [[nodiscard]] virtual std::size_t num_actions() const = 0;
    [[maybe_unused]] virtual void reset() = 0;
    [[maybe_unused]] [[nodiscard]] virtual bool is_game_over() const = 0;
    [[maybe_unused]] [[nodiscard]] virtual float score() const = 0;
    [[maybe_unused]] [[nodiscard]] virtual std::size_t state_id() const = 0;
    [[maybe_unused]] [[nodiscard]] virtual bool is_forbidden(std::size_t action) const = 0;
    [[maybe_unused]] [[nodiscard]] virtual std::vector<std::size_t> available_actions() const = 0;
    [[maybe_unused]] virtual void step(std::size_t action) = 0;
};
