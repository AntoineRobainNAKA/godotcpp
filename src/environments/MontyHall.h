#include "../contracts/ModelFreeEnv.h"
#include <random>
#include <stdexcept>
#include <algorithm>

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
        , removed_door(0)
        , made_initial_choice(false)
        , game_finished(false)
        , current_score(0.0f)
        , rng(std::random_device{}()) {
        if (num_doors < 3) {
            throw std::invalid_argument("Number of doors must be at least 3");
        }
    }

    size_t num_states() const override {
        // States: (num_doors choose 1) * (num_doors choose 1) * (num_doors-2 choose 1)
        // For 3 doors: 3 * 3 * 1 = 9 states
        return num_doors * num_doors * (num_doors - 2);
    }

    size_t num_actions() const override {
        return num_doors; // For both initial choice and switch/stay decision
    }

    void reset() override {
        // Randomly select winning door
        std::uniform_int_distribution<size_t> dist(0, num_doors - 1);
        winning_door = dist(rng);
        made_initial_choice = false;
        game_finished = false;
        current_score = 0.0f;
        initial_choice = 0;
        removed_door = 0;
    }

    bool is_game_over() const override {
        return game_finished;
    }

    float score() const override {
        return current_score;
    }

    size_t state_id() const override {
        if (!made_initial_choice) {
            return 0; // Initial state
        }
        // Encode state based on winning_door, initial_choice, and removed_door
        return 1 + winning_door * num_doors * (num_doors - 2) +
               initial_choice * (num_doors - 2) + removed_door;
    }

    bool is_forbidden(size_t action) const override {
        if (action >= num_doors) {
            return true;
        }
        if (made_initial_choice) {
            // Can only choose initial_choice (stay) or the remaining door (switch)
            return action != initial_choice && action != get_remaining_door();
        }
        return false;
    }

    std::vector<size_t> available_actions() const override {
        std::vector<size_t> actions;
        if (!made_initial_choice) {
            // First choice: all doors available
            actions.reserve(num_doors);
            for (size_t i = 0; i < num_doors; ++i) {
                actions.push_back(i);
            }
        } else if (!game_finished) {
            // Second choice: only initial door or remaining door
            actions.push_back(initial_choice);
            actions.push_back(get_remaining_door());
        }
        return actions;
    }

    void step(size_t action) override {
        if (is_forbidden(action)) {
            throw std::invalid_argument("Invalid action");
        }

        if (!made_initial_choice) {
            // First choice
            initial_choice = action;
            made_initial_choice = true;
            
            // Host removes a non-winning door different from the initial choice
            std::vector<size_t> removable_doors;
            for (size_t i = 0; i < num_doors; ++i) {
                if (i != initial_choice && i != winning_door) {
                    removable_doors.push_back(i);
                }
            }
            std::uniform_int_distribution<size_t> dist(0, removable_doors.size() - 1);
            removed_door = removable_doors[dist(rng)];
        } else {
            // Final choice
            game_finished = true;
            current_score = (action == winning_door) ? 1.0f : 0.0f;
        }
    }

private:
    size_t get_remaining_door() const {
        for (size_t i = 0; i < num_doors; ++i) {
            if (i != initial_choice && i != removed_door) {
                return i;
            }
        }
        throw std::runtime_error("No remaining door found");
    }
};

// Implementation of the factory methods
std::unique_ptr<ModelFreeEnv> ModelFreeEnv::create() {
    return std::make_unique<MontyHallEnv>();
}

std::unique_ptr<ModelFreeEnv> ModelFreeEnv::from_random_state(std::mt19937& rng) {
    auto env = std::make_unique<MontyHallEnv>();
    env->reset();
    return env;
}