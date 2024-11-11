#include <cstddef>
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "../contracts/MDPEnv.h"
#include "../contracts/ModelFreeEnv.h"

template <std::size_t ROWS, std::size_t COLS>
class [[maybe_unused]] GridWorld : public MDPEnv, public ModelFreeEnv {
private:
    std::size_t current_row;
    std::size_t current_col;
    std::size_t goal_row;
    std::size_t goal_col;

    enum Action {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

public:
    GridWorld() : current_row(0), current_col(0), goal_row(ROWS - 1), goal_col(COLS - 1) {}

    explicit GridWorld(std::size_t initial_row, std::size_t inital_col, std::size_t goal_row, std::size_t goal_col)
        : current_row(initial_row), current_col(inital_col), goal_row(goal_row), goal_col(goal_col) {} 

    void reset() override {
        current_row = 0;
        current_col = 0;
    }

    // MDPEnv methods implementations
    [[nodiscard]] std::size_t num_states() const override {
        return ROWS * COLS;
    }

    [[nodiscard]] std::size_t num_actions() const override {
        return 4; // Up, Down, Left, Right
    }

    [[nodiscard]] std::size_t num_rewards() const override {
        return 2;
    }

    [[nodiscard]] float reward(std::size_t index) const override {
        switch (index) {
            case 0:
                return -0.01f; // Penalty for non-goal states
            case 1:
                return 1.0f; // Reward for reaching the goal state
            default:
                throw std::invalid_argument("Invalid reward index");
        }
    }

    [[nodiscard]] float transition_probability(std::size_t state, std::size_t action,
                                                std::size_t next_state, std::size_t reward_index) const override {
        int current_row = state / COLS;
        int current_col = state % COLS;

        int next_row = current_row;
        int next_col = current_col;
        
        if (action == UP) {
            next_row = std::max(0, current_row - 1);
        } else if (action == DOWN) {
            next_row = std::min(static_cast<int>(ROWS - 1), current_row + 1);
        } else if (action == LEFT) {
             next_col = std::max(0, current_col - 1);
        } else if (action == RIGHT) {
            next_col = std::min(static_cast<int>(COLS - 1), current_col + 1);
        }

        int next_state_index = next_row * COLS + next_col;

        if (next_state_index == next_state) {
            int reward = 0;

            if (next_row == goal_row && next_col == goal_col) {
                reward = 1;
            } else {
                reward = 0;
            }

            if (reward_index == static_cast<std::size_t>(reward)) {
                return 1.0f;
            }
        }

        return 0.0f;
    }

    // ModelFreeEnv methods implementation
    [[nodiscard]] bool is_game_over() const override {
        return (current_row == goal_row && current_col == goal_col);
    }

    [[nodiscard]] float score() const override {
        return reward(is_game_over() ? 1.0f : -0.1f);
    }

    [[nodiscard]] std::size_t state_id() const override {
        return current_row * COLS + current_col;
    }
    
    [[nodiscard]] bool is_forbidden(std::size_t action) const override {
        return false; // all actions allowed
    }

    [[nodiscard]] std::vector<std::size_t> available_actions() const override{
        std::vector<std::size_t> actions;
        actions.push_back(UP);
        actions.push_back(DOWN);
        actions.push_back(LEFT);
        actions.push_back(RIGHT);
        return actions;
    } 

    void step(std::size_t action) override {
        switch (action)
        {
        case UP:
            if (current_row > 0) {
                current_row--;
            }
            break;
        
        case DOWN:
            if (current_row  < ROWS - 1) {
                current_row++;
            }
            break;
        
        case LEFT:
            if (current_col > 0) {
                current_col--;
            }
            break;

        case RIGHT:
            if (current_col < COLS - 1) {
                current_col++;
            }
        default:
            break;
        }
    }
};