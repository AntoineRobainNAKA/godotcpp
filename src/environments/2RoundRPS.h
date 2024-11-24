#include <cstddef>
#include <vector>
#include <memory>
#include <random>
#include <stdexcept>
#include "../contracts/MDPEnv.h"
#include "../contracts/ModelFreeEnv.h"

class RockPaperScissors : public MDPEnv, public ModelFreeEnv {
private:
    // Game state variables
    std::size_t current_round;     
    std::size_t agent_last_move;   
    std::size_t opponent_move;     
    bool game_finished;
    float total_score;
    
    mutable std::random_device rd;
    mutable std::mt19937 gen;
    mutable std::uniform_int_distribution<> dis;

    enum Move {
        ROCK = 0,
        PAPER = 1,
        SCISSORS = 2
    };

    float calculate_reward(std::size_t agent_move, std::size_t opponent_move) const {
        if (agent_move == opponent_move) return 0.0f;
        
        if ((agent_move == ROCK && opponent_move == SCISSORS) ||
            (agent_move == SCISSORS && opponent_move == PAPER) ||
            (agent_move == PAPER && opponent_move == ROCK)) {
            return 1.0f;
        }
        
        return -1.0f;
    }

public:
    RockPaperScissors() 
        : current_round(0), agent_last_move(0), opponent_move(0), 
          game_finished(false), total_score(0.0f),
          gen(rd()), dis(0, 2) {
        reset();
    }

    void reset() override {
        current_round = 0;
        agent_last_move = 0;
        opponent_move = dis(gen);
        game_finished = false;
        total_score = 0.0f;
    }

    [[nodiscard]] std::size_t num_states() const override {
        return 12;
    }

    [[nodiscard]] std::size_t num_actions() const override {
        return 3;
    }

    [[nodiscard]] std::size_t num_rewards() const override {
        return 3;
    }

    [[nodiscard]] float reward(std::size_t index) const override {
        switch (index) {
            case 0: return -1.0f;
            case 1: return 0.0f;
            case 2: return 1.0f;
            default: throw std::invalid_argument("Invalid reward index");
        }
    }

    [[nodiscard]] float transition_probability(std::size_t state, std::size_t action,
                                             std::size_t next_state, std::size_t reward_index) const override {
        bool is_first_round = state < 3;
        
        if (is_first_round) {
            std::size_t opponent_current_move = state;
            float round_reward = calculate_reward(action, opponent_current_move);
            
            std::size_t expected_reward_index;
            if (round_reward > 0) expected_reward_index = 2;
            else if (round_reward < 0) expected_reward_index = 0;
            else expected_reward_index = 1;
            
            std::size_t expected_next_state = 3 + (action * 3) + opponent_current_move;
            
            if (next_state == expected_next_state && reward_index == expected_reward_index) {
                return 1.0f;
            }
        } else {
            std::size_t agent_first_move = (state - 3) / 3;
            
            std::size_t opponent_second_move = agent_first_move;
            
            float round_reward = calculate_reward(action, opponent_second_move);
            
            std::size_t expected_reward_index;
            if (round_reward > 0) expected_reward_index = 2;
            else if (round_reward < 0) expected_reward_index = 0;
            else expected_reward_index = 1;
            
            // In second round, state doesn't change (game ends)
            if (next_state == state && reward_index == expected_reward_index) {
                return 1.0f;
            }
        }
        
        return 0.0f;
    }

    [[nodiscard]] bool is_game_over() const override {
        return game_finished;
    }

    [[nodiscard]] float score() const override {
        return total_score;
    }

    [[nodiscard]] std::size_t state_id() const override {
        if (current_round == 0) {
            return opponent_move;
        } else {
            // State encoding: 3 + (first move * 3) + opponent's first move
            return 3 + (agent_last_move * 3) + opponent_move;
        }
    }

    [[nodiscard]] bool is_forbidden(std::size_t action) const override {
        return false;
    }

    [[nodiscard]] std::vector<std::size_t> available_actions() const override {
        return {ROCK, PAPER, SCISSORS};
    }

    void step(std::size_t action) override {
        if (is_game_over()) return;

        float round_reward = calculate_reward(action, opponent_move);
        total_score += round_reward;

        if (current_round == 0) {
            agent_last_move = action;
            std::size_t opponent_first_move = opponent_move;
            // Set opponent's second move to be agent's first move
            opponent_move = action;
            current_round = 1;
        } else {
            game_finished = true;
        }
    }
};