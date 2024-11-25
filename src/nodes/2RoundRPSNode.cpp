#pragma once

#include "2RoundRPSNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"
#include "algorithms/MonteCarloES.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "algorithms/OnPolicyFirstVisitMonteCarlo.h"
#include "algorithms/DynaQ.h"
#include <sstream>

using namespace godot;

void RPSNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_algorithm", "algorithm_type", "gamma", "theta", "num_episodes", "learning_rate", "epsilon"), &RPSNode::launch_algorithm);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &RPSNode::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &RPSNode::get_result);
}

RPSNode::RPSNode() {
    calculation_pending = false;
}

RPSNode::~RPSNode() {
    if (calculation_pending && current_calculation.valid()) {
        current_calculation.wait(); // Clean up any pending calculation
    }
}

void RPSNode::launch_algorithm(int algorithm_type, float gamma, float theta, int num_episodes, float learning_rate, float epsilon) {
    // Check if a calculation is already in progress
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }    
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, [this, algorithm_type, gamma, theta, num_episodes, learning_rate, epsilon]() {
        RockPaperScissors rps;
        std::stringstream ss;

        switch (algorithm_type) {
            case 1: {
                auto [pi_rps, value_function_rps] = policy_iteration(rps, gamma, theta);

                ss << "Policy Iteration Results:\n";
                for (std::size_t s = 0; s < pi_rps.size(); ++s) {
                    std::string state_desc;
                    if (s < 3) {
                        state_desc = "Round 1, Opponent: " + std::string(s == 0 ? "Rock" : s == 1 ? "Paper" : "Scissors");
                    } else {
                        std::size_t agent_prev = (s - 3) / 3;
                        std::size_t opp_prev = (s - 3) % 3;
                        state_desc = "Round 2, Agent played: " + std::string(agent_prev == 0 ? "Rock" : agent_prev == 1 ? "Paper" : "Scissors") +
                                   ", Opponent played: " + std::string(opp_prev == 0 ? "Rock" : opp_prev == 1 ? "Paper" : "Scissors");
                    }
                    ss << "State " << s << " (" << state_desc << "):\n";
                    ss << "  Policy: " << (pi_rps[s] == 0 ? "Rock" : pi_rps[s] == 1 ? "Paper" : "Scissors") << "\n";
                    ss << "  Value: " << value_function_rps[s] << "\n\n";
                }
                break;
            }
            case 2: {
                auto q_values_rps = q_learning(rps, num_episodes, learning_rate, gamma, epsilon);

                ss << "Q-Learning Results:\n";
                for (std::size_t s = 0; s < q_values_rps.size(); ++s) {
                    std::string state_desc;
                    if (s < 3) {
                        state_desc = "Round 1, Opponent: " + std::string(s == 0 ? "Rock" : s == 1 ? "Paper" : "Scissors");
                    } else {
                        std::size_t agent_prev = (s - 3) / 3;
                        std::size_t opp_prev = (s - 3) % 3;
                        state_desc = "Round 2, Agent played: " + std::string(agent_prev == 0 ? "Rock" : agent_prev == 1 ? "Paper" : "Scissors") +
                                   ", Opponent played: " + std::string(opp_prev == 0 ? "Rock" : opp_prev == 1 ? "Paper" : "Scissors");
                    }
                    ss << "State " << s << " (" << state_desc << "):\n";
                    for (std::size_t a = 0; a < q_values_rps[s].size(); ++a) {
                        ss << "  Q(a=" << (a == 0 ? "Rock" : a == 1 ? "Paper" : "Scissors") 
                           << ") = " << q_values_rps[s][a] << "\n";
                    }
                    ss << "\n";
                }
                break;
            }
            case 3: {
                auto value_function_rps = value_iteration(rps, gamma, theta);

                ss << "Value Iteration Results:\n";
                for (std::size_t s = 0; s < value_function_rps.size(); ++s) {
                    std::string state_desc;
                    if (s < 3) {
                        state_desc = "Round 1, Opponent: " + std::string(s == 0 ? "Rock" : s == 1 ? "Paper" : "Scissors");
                    } else {
                        std::size_t agent_prev = (s - 3) / 3;
                        std::size_t opp_prev = (s - 3) % 3;
                        state_desc = "Round 2, Agent played: " + std::string(agent_prev == 0 ? "Rock" : agent_prev == 1 ? "Paper" : "Scissors") +
                                   ", Opponent played: " + std::string(opp_prev == 0 ? "Rock" : opp_prev == 1 ? "Paper" : "Scissors");
                    }
                    ss << "State " << s << " (" << state_desc << "):\n";
                    ss << "  Value: " << value_function_rps[s] << "\n\n";
                }
                break;
            }
            case 4: {
                auto q_values_rps = monte_carlo_es(rps, num_episodes, epsilon);

                for (std::size_t s = 0; s < q_values_rps.size(); ++s) {
                    const auto& q_s = q_values_rps[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 5: {
                auto [q_values_rps, returns_sum_rps] = on_policy_first_visit_monte_carlo_control(
                    rps,
                    num_episodes,
                    gamma,
                    epsilon
                );

                for (std::size_t s = 0; s < q_values_rps.size(); ++s) {
                    const auto& q_s = q_values_rps[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 6: {
                auto q_values_rps = dina_q(rps, num_episodes, learning_rate, gamma, epsilon, 100, 32, 10);

                for (std::size_t state = 0; state < q_values_rps.size(); ++state) {
                    ss << "État " << state << ": ";
                    for (float q_value : q_values_rps[state]) {
                        ss << q_value << " ";
                    }
                ss << std::endl;
                }
            }
            default:
                ss << "No algorithm selected";
                break;
        }

        return String(ss.str().c_str());
    });
}

bool RPSNode::is_calculation_complete() const {
    if (!calculation_pending) return false;
    if (!current_calculation.valid()) return false;

    auto status = current_calculation.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

String RPSNode::get_result() {
    if (!calculation_pending || !current_calculation.valid()) {
        return String("No calculation in progress.");
    }

    cached_result = current_calculation.get();
    calculation_pending = false;
    return cached_result;
}

void RPSNode::_process(double delta) {
    // Nothing needed here
}