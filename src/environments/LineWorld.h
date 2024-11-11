//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_LINEWORLD_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_LINEWORLD_H

#include <cstddef>
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "../contracts/MDPEnv.h"
#include "../contracts/ModelFreeEnv.h"

template <std::size_t NB_CELLS>
class [[maybe_unused]] LineWorld : public MDPEnv, public ModelFreeEnv {
private:
    std::size_t current_cell;

public:
    LineWorld() : current_cell(NB_CELLS / 2) {}

    explicit LineWorld(std::size_t initial_cell) : current_cell(initial_cell) {}

    [[maybe_unused]] static std::unique_ptr<LineWorld> create() {
        return std::make_unique<LineWorld>();
    }

    template <typename RNG>
    [[maybe_unused]] static std::unique_ptr<LineWorld> from_random_state(RNG& rng) {
        std::uniform_int_distribution<std::size_t> dist(0, NB_CELLS - 1);
        return std::make_unique<LineWorld>(dist(rng));
    }

    // Implémentation des méthodes de MDPEnv
    [[nodiscard]] std::size_t num_states() const override {
        return NB_CELLS;
    }

    [[nodiscard]] std::size_t num_actions() const override {
        return 2;
    }

    [[nodiscard]] std::size_t num_rewards() const override {
        return 3;
    }

    [[nodiscard]] float reward(std::size_t index) const override {
        switch (index) {
            case 0: return -1.0f;
            case 1: return  0.0f;
            case 2: return  1.0f;
            default: throw std::out_of_range("Invalid reward index");
        }
    }

    [[nodiscard]] float transition_probability(std::size_t state, std::size_t action,
                                 std::size_t next_state, std::size_t reward_index) const override {
        if (state == 0 || state == NB_CELLS - 1) {
            return 0.0f;
        }

        if (action == 1 && reward_index == 1 && (state >= 1 && state <= NB_CELLS - 3) && next_state == state + 1) {
            return 1.0f;
        }

        if (action == 0 && reward_index == 1 && (state >= 2 && state <= NB_CELLS - 2) && next_state == state - 1) {
            return 1.0f;
        }

        if (state == 1 && action == 0 && next_state == 0 && reward_index == 0) {
            return 1.0f;
        }

        if (state == NB_CELLS - 2 && action == 1 && next_state == NB_CELLS - 1 && reward_index == 2) {
            return 1.0f;
        }

        return 0.0f;
    }

    // Implémentation des méthodes de ModelFreeEnv
    void reset() override {
        current_cell = NB_CELLS / 2;
    }

    [[nodiscard]] bool is_game_over() const override {
        return current_cell == 0 || current_cell == NB_CELLS - 1;
    }

    [[nodiscard]] float score() const override {
        if (current_cell == 0) {
            return -1.0f;
        } else if (current_cell == NB_CELLS - 1) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    }

    [[nodiscard]] std::size_t state_id() const override {
        return current_cell;
    }

    [[nodiscard]] bool is_forbidden(std::size_t action) const override {
        return is_game_over() || action > 1;
    }

    [[nodiscard]] std::vector<std::size_t> available_actions() const override {
        if (is_game_over()) {
            return {};
        } else {
            return {0, 1};
        }
    }

    void step(std::size_t action) override {
        if (is_forbidden(action)) {
            std::cerr << "Forbidden action" << std::endl;
            std::exit(42);
        }

        if (action == 0) {
            current_cell -= 1;
        } else if (action == 1) {
            current_cell += 1;
        } else {
            throw std::invalid_argument("Invalid action, should not happen");
        }
    }
};


#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_LINEWORLD_H
