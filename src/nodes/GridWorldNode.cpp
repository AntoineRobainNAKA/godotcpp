#include "GridWorldNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"
#include "../algorithms/MonteCarloES.h"
#include "../algorithms/OnPolicyFirstVisitMonteCarlo.h"
#include "algorithms/DinaQ.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sstream>

using namespace godot;

void GridWorldNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_algorithm", "algorithm_type", "rows", "columns", "gamma", "theta", "num_episodes", "learning_rate", "epsilon"), &GridWorldNode::launch_algorithm);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &GridWorldNode::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &GridWorldNode::get_result);
}

GridWorldNode::GridWorldNode() {
    rows = 5;
    columns = 5;
    calculation_pending = false;
}

GridWorldNode::~GridWorldNode() {
    rows = 0;
    columns = 0;

    if (calculation_pending && current_calculation.valid()) {
        current_calculation.wait(); // Clean up any pending calculation
    }
}

void GridWorldNode::launch_algorithm(int algorithm_type, const int rows, const int columns, float gamma, float theta, int num_episodes, float learning_rate, float epsilon) {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }
    calculation_pending = true;

    // Use a lambda to capture the configuration and choose the algorithm
    current_calculation = std::async(std::launch::async, [this, algorithm_type, rows, columns, gamma, theta, num_episodes, learning_rate, epsilon]() {
        GridWorld gridworld(rows, columns);
        std::stringstream ss;

        switch (algorithm_type) {
            case 1: {
                auto [pi_gridworld, value_function_gridworld] = policy_iteration(
                    gridworld, 
                    gamma, 
                    theta
                );

                for (std::size_t s = 0; s < pi_gridworld.size(); ++s) {
                    ss << "Pi(s=" << s << ") = " << pi_gridworld[s] << std::endl;
                }
                for (std::size_t s = 0; s < value_function_gridworld.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_gridworld[s] << std::endl;
                }
                break;
            }
            case 2: {
                auto q_values_gridworld = q_learning(gridworld, num_episodes, learning_rate, gamma, epsilon);

                for (std::size_t s = 0; s < q_values_gridworld.size(); ++s) {
                    const auto &q_s = q_values_gridworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 3: {
                auto value_function_gridworld = value_iteration(gridworld, gamma, theta);

                for (std::size_t s = 0; s < value_function_gridworld.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_gridworld[s] << std::endl;
                }
                break;
            }
            case 4: {
                auto q_values_gridworld = monte_carlo_es(gridworld, num_episodes, epsilon);

                for (std::size_t s = 0; s < q_values_gridworld.size(); ++s) {
                    const auto& q_s = q_values_gridworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 5: {
                auto [q_values_gridworld, returns_sum_gridworld] = on_policy_first_visit_monte_carlo_control(
                    gridworld,
                    num_episodes,
                    gamma,
                    epsilon
                );

                for (std::size_t s = 0; s < q_values_gridworld.size(); ++s) {
                    const auto& q_s = q_values_gridworld[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            default:
                ss << "No algorithm selected";
                break;
        }

        return String(ss.str().c_str());
    });
}

bool GridWorldNode::is_calculation_complete() const {
    if (!calculation_pending) return false;
    if (!current_calculation.valid()) return false;

    auto status = current_calculation.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

String GridWorldNode::get_result() {
    if (!calculation_pending || !current_calculation.valid()) {
        return String("No calculation in progress.");
    }

    cached_result = current_calculation.get();
    calculation_pending = false;
    return cached_result;
}

// Obligé de mettre _process sinon ça compile pas
void GridWorldNode::_process(double delta) {
    //nothing for now I guess
}