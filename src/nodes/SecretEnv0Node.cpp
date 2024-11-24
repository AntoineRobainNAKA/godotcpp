#pragma once

#include "SecretEnv0Node.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sstream>

using namespace godot;

void SecretEnv0Node::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_algorithm", "algorithm_type"), &SecretEnv0Node::launch_algorithm);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &SecretEnv0Node::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &SecretEnv0Node::get_result);
}

SecretEnv0Node::SecretEnv0Node() {
    calculation_pending = false;
}

SecretEnv0Node::~SecretEnv0Node() {
    if (calculation_pending && current_calculation.valid()) {
        current_calculation.wait(); // Clean up any pending calculation
    }
}

void SecretEnv0Node::launch_algorithm(int algorithm_type) {
    // Check if a calculation is already in progress
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }    
    calculation_pending = true;

    // Use a lambda to capture the configuration and choose the algorithm
    current_calculation = std::async(std::launch::async, [this, algorithm_type]() {
        SecretEnv0 env0;
        std::stringstream ss;

        switch (algorithm_type) {
            case 1: {
                auto [pi_env0, value_function_env0] = policy_iteration(
                    env0, 
                    0.9f, 
                    0.0001f
                );

                for (std::size_t s = 0; s < pi_env0.size(); ++s) {
                    ss << "Pi(s=" << s << ") = " << pi_env0[s] << std::endl;
                }
                for (std::size_t s = 0; s < value_function_env0.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_env0[s] << std::endl;
                }
                break;
            }
            case 2: {
                auto q_values_env0 = q_learning(
                    env0, 
                    10000, 
                    0.1f, 
                    0.9f, 
                    1.0f
                );

                for (std::size_t s = 0; s < q_values_env0.size(); ++s) {
                    const auto &q_s = q_values_env0[s];
                    for (std::size_t a = 0; a < q_s.size(); ++a) {
                        ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
                    }
                }
                break;
            }
            case 3: {
                auto value_function_env0 = value_iteration(
                    env0, 
                    0.9f, 
                    0.0001f
                );

                for (std::size_t s = 0; s < value_function_env0.size(); ++s) {
                    ss << "V(s=" << s << ") = " << value_function_env0[s] << std::endl;
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

bool SecretEnv0Node::is_calculation_complete() const {
    if (!calculation_pending) return false;
    if (!current_calculation.valid()) return false;

    auto status = current_calculation.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

String SecretEnv0Node::get_result() {
    if (!calculation_pending || !current_calculation.valid()) {
        return String("No calculation in progress.");
    }

    cached_result = current_calculation.get();
    calculation_pending = false;
    return cached_result;
}

// Obligé de mettre _process sinon ça compile pas
void SecretEnv0Node::_process(double delta) {
    //nothing for now I guess
}