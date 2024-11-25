#include "LineWorldNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "../algorithms/MonteCarloES.h"
#include <sstream>
#include "../algorithms/OnPolicyFirstVisitMonteCarlo.h"
#include "algorithms/DynaQ.h"

using namespace godot;

void LineWorldNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_algorithm", "algorithm_type", "cells", "gamma", "theta", "num_episodes", "learning_rate", "epsilon"), &LineWorldNode::launch_algorithm);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &LineWorldNode::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &LineWorldNode::get_result);
}

LineWorldNode::LineWorldNode() {
    cells = 5;
    calculation_pending = false;
}

LineWorldNode::~LineWorldNode() {
    cells = 0;

    if (calculation_pending && current_calculation.valid()) {
        current_calculation.wait(); // Clean up any pending calculation
    }
}

void LineWorldNode::launch_algorithm(int algorithm_type, const int cells, float gamma, float theta, int num_episodes, float learning_rate, float epsilon) {
    // Check if a calculation is already in progress
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }    
    calculation_pending = true;

    // Use a lambda to capture the configuration and choose the algorithm
    current_calculation = std::async(std::launch::async, [this, algorithm_type, cells, gamma, theta, num_episodes, learning_rate, epsilon]() {
        LineWorld lineworld(cells);
        std::stringstream ss;

        switch (algorithm_type) {
            case 1: {
                auto [pi_lineworld, value_function_lineworld] = policy_iteration(
                    lineworld, 
                    gamma, 
                    theta
                );

                for (std::size_t s = 0; s < pi_lineworld.size(); ++s) {
                    ss << "Pi(s=" << s << ") = " << pi_lineworld[s] << std::endl;
                }
                for (std::size_t s = 0; s < value_function_lineworld.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_lineworld[s] << std::endl;
                }
                break;
            }
            case 2: {
                auto q_values_lineworld = q_learning(lineworld, num_episodes, learning_rate, gamma, epsilon);

                for (std::size_t s = 0; s < q_values_lineworld.size(); ++s) {
                    const auto &q_s = q_values_lineworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 3: {
                auto value_function_lineworld = value_iteration(lineworld, gamma, theta);

                for (std::size_t s = 0; s < value_function_lineworld.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_lineworld[s] << std::endl;
                }
                break;
            }
            case 4: {
                auto q_values_lineworld = monte_carlo_es(lineworld, num_episodes, epsilon);

                for (std::size_t s = 0; s < q_values_lineworld.size(); ++s) {
                    const auto& q_s = q_values_lineworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 5: {
                auto [q_values_lineworld, returns_sum_lineworld] = on_policy_first_visit_monte_carlo_control(
                    lineworld,
                    num_episodes,
                    gamma,
                    epsilon
                );
                for (std::size_t s = 0; s < q_values_lineworld.size(); ++s) {
                    const auto& q_s = q_values_lineworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 6: {
                auto q_values_lineworld = dina_q(lineworld, num_episodes, learning_rate, gamma, epsilon, 100, 32, 10);

                for (std::size_t state = 0; state < q_values_lineworld.size(); ++state) {
                    ss << "État " << state << ": ";
                    for (float q_value : q_values_lineworld[state]) {
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

bool LineWorldNode::is_calculation_complete() const {
    if (!calculation_pending) return false;
    if (!current_calculation.valid()) return false;

    auto status = current_calculation.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

String LineWorldNode::get_result() {
    if (!calculation_pending || !current_calculation.valid()) {
        return String("No calculation in progress.");
    }

    cached_result = current_calculation.get();
    calculation_pending = false;
    return cached_result;
}

// Obligé de mettre _process sinon ça compile pas
void LineWorldNode::_process(double delta) {
    //nothing for now I guess
}