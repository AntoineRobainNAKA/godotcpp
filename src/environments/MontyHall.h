#pragma once

#include "../contracts/ModelFreeEnv.h"
#include <random>
#include <stdexcept>
#include <algorithm>
#include <iostream>

class MontyHallEnv : public ModelFreeEnv {
private:
    const size_t num_doors;
    size_t winning_door;
    size_t initial_choice;
    size_t removed_door;
    bool made_initial_choice;
    bool game_finished;
    float current_score;
    std::mt19937 rng;

public:
    explicit MontyHallEnv(size_t n_doors = 3) 
        : num_doors(n_doors)
        , winning_door(0)
        , initial_choice(0)
        , removed_door(num_doors)
        , made_initial_choice(false)
        , game_finished(false)
        , current_score(0.0f)
        , rng(std::random_device{}()) {
        if (num_doors < 3) {
            throw std::invalid_argument("Number of doors must be at least 3");
        }
    }

    size_t num_states() const override {
        // States:
        // 1 initial state (before first choice) +
        // num_doors (winning door) * num_doors (initial choice) states after first choice +
        // num_doors (winning door) * num_doors (initial choice) states after door removal
        return 1 + (num_doors * num_doors) * 2;
    }

    size_t state_id() const override {
        // If the player has not made an initial choice, return state 0 (initial state)
        if (!made_initial_choice) {
            return 0;
        }

        // Base offset for states after the initial choice has been made
        size_t base = 1;

        // Calculate total number of states before door removal
        size_t total_states_before_removal = num_doors * num_doors;

        // If a door has been removed, add the offset for all states before door removal
        if (removed_door != num_doors) {
            base += total_states_before_removal;
        }

        // Now we need to calculate a unique ID based on the winning door, initial choice, and removed door.
        // The formula for the state ID is:
        // state_id = base + (winning_door * num_doors^2) + (initial_choice * num_doors) + removed_door
        
        // If no door has been removed yet, we don't use `removed_door`, so we'll only account for
        // `winning_door` and `initial_choice`. When the door is removed, it's factored in.

        // After removal, `removed_door` should always be a valid door index. If no door is removed yet,
        // we only need to account for `winning_door` and `initial_choice`.

        return base + (winning_door * num_doors) + initial_choice;
    }
    
    size_t num_actions() const override {
        return num_doors;
    }

    void reset() override {
        std::uniform_int_distribution<size_t> dist(0, num_doors - 1);
        winning_door = dist(rng);
        made_initial_choice = false;
        game_finished = false;
        current_score = 0.0f;
        initial_choice = 0;
        removed_door = num_doors;
    }

    bool is_game_over() const override {
        return game_finished;
    }

    float score() const override {
        return current_score;
    }

    bool is_forbidden(size_t action) const override {
        if (action >= num_doors) {
            return true;
        }
        
        if (made_initial_choice) {
            if (removed_door == num_doors) {
                return false;
            }
            return (action == removed_door) || 
                   (action != initial_choice && action != get_remaining_door());
        }
        
        return false;
    }

    std::vector<size_t> available_actions() const override {
        std::vector<size_t> actions;
        if (!made_initial_choice) {
            actions.reserve(num_doors);
            for (size_t i = 0; i < num_doors; ++i) {
                actions.push_back(i);
            }
        } else if (!game_finished) {
            if (removed_door != num_doors) {
                actions.push_back(initial_choice);
                actions.push_back(get_remaining_door());
            } else {
                for (size_t i = 0; i < num_doors; ++i) {
                    if (i != initial_choice) {
                        actions.push_back(i);
                    }
                }
            }
        }
        return actions;
    }

    void step(size_t action) override {
        if (!made_initial_choice) {
            initial_choice = action;
            made_initial_choice = true;
            
            std::vector<size_t> removable_doors;
            for (size_t i = 0; i < num_doors; ++i) {
                if (i != initial_choice && i != winning_door) {
                    removable_doors.push_back(i);
                }
            }
            
            std::uniform_int_distribution<size_t> dist(0, removable_doors.size() - 1);
            removed_door = removable_doors[dist(rng)];
        } else {
            game_finished = true;
            current_score = (action == winning_door) ? 1.0f : 0.0f;
        }
    }

    std::size_t get_removed_door() const {
        return removed_door;
    }

private:
    size_t get_remaining_door() const {
        for (size_t i = 0; i < num_doors; ++i) {
            if (i != initial_choice && i != removed_door) {
                return i;
            }
        }
        return 0.0f;
    }
};