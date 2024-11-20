#include <cstddef>
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "../contracts/MDPEnv.h"
#include "../contracts/ModelFreeEnv.h"

class GridWorld : public MDPEnv, public ModelFreeEnv {
private:
    std::size_t rows;
    std::size_t cols;
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
    GridWorld(std::size_t r, std::size_t c) 
        : rows(r), cols(c), 
          current_row(0), current_col(0), 
          goal_row(r - 1), goal_col(c - 1) {}

    void reset() override {
        current_row = 0;
        current_col = 0;
    }

    // MDPEnv methods implementations
    [[nodiscard]] std::size_t num_states() const override {
        return rows * cols;
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
        int current_row = state / cols;
        int current_col = state % cols;

        int next_row = current_row;
        int next_col = current_col;

        // Determine the intended next position based on the action
        switch (action) {
            case UP:    next_row = current_row - 1; break;
            case DOWN:  next_row = current_row + 1; break;
            case LEFT:  next_col = current_col - 1; break;
            case RIGHT: next_col = current_col + 1; break;
        }

        // Check if the intended next position is valid
        if (next_row < 0 || next_row >= static_cast<int>(rows) ||
            next_col < 0 || next_col >= static_cast<int>(cols)) {
            // If the intended position is out of bounds, the agent stays in the current position
            next_row = current_row;
            next_col = current_col;
        }

        std::size_t computed_next_state = next_row * cols + next_col;

        // Calculate the transition probability
        if (computed_next_state == next_state) {
            // If the agent reaches the goal state, return 1.0 for the goal reward index
            if (next_row == goal_row && next_col == goal_col) {
                return (reward_index == 1) ? 1.0f : 0.0f;
            }
            // If the agent doesn't reach the goal, return 1.0 for the non-goal reward index
            else {
                return (reward_index == 0) ? 1.0f : 0.0f;
            }
        }

        // If the next state doesn't match the computed next state, the transition probability is 0.0
        return 0.0f;
    }

    // ModelFreeEnv methods implementation
    [[nodiscard]] bool is_game_over() const override {
        return (current_row == goal_row && current_col == goal_col);
    }

    [[nodiscard]] float score() const override {
        return reward(is_game_over() ? 1 : 0);
    }

    [[nodiscard]] std::size_t state_id() const override {
        return current_row * cols + current_col;
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
            if (current_row  < rows - 1) {
                current_row++;
            }
            break;
        
        case LEFT:
            if (current_col > 0) {
                current_col--;
            }
            break;

        case RIGHT:
            if (current_col < cols - 1) {
                current_col++;
            }
            break;
        default:
            break;
        }
    }
};