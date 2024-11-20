#include "LineWorldNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void LineWorldNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_policy_iteration"), &LineWorldNode::launch_policy_iteration);
    ClassDB::bind_method(D_METHOD("launch_q_learning"), &LineWorldNode::launch_q_learning);
    ClassDB::bind_method(D_METHOD("launch_value_iteration"), &LineWorldNode::launch_value_iteration);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &LineWorldNode::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &LineWorldNode::get_result);
}

LineWorldNode::LineWorldNode() {
    rows = 5;
    columns = 5;
    calculation_pending = false;
}

LineWorldNode::~LineWorldNode() {
    rows = 0;
    columns = 0;

    if (calculation_pending && current_calculation.valid()) {
        current_calculation.wait(); // Clean up any pending calculation
    }
}

// normalement ce qui est fourni par le prof est bon
void LineWorldNode::launch_policy_iteration(const int cells) {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progres !");
        return;
    }    
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, [cells]() {
        // Pas trouvé le moyen de passer la taille en paramètre
        LineWorld lineworld(cells);
        auto [pi_lineworld, value_function_lineworld] = policy_iteration(lineworld, 0.999f, 0.001f);

        std::stringstream ss;

        for (std::size_t s = 0; s < pi_lineworld.size(); ++s) {
            ss << "Pi(s=" << s << ") = " << pi_lineworld[s] << std::endl;
        }
        for (std::size_t s = 0; s < value_function_lineworld.size(); ++s) {
            ss << "V(s=" << s << ") = " << value_function_lineworld[s] << std::endl;
        }
        
        return String(ss.str().c_str());
    });
}

void LineWorldNode::launch_q_learning(const int cells) {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress !");
        return;
    }    
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, [cells]() {
        LineWorld lineworld(cells);
        auto q_values_lineworld = q_learning(lineworld, 10000, 0.1f, 0.999f, 1.0f);
        std::stringstream ss;

        for (std::size_t s = 0; s < q_values_lineworld.size(); ++s) {
            const auto &q_s = q_values_lineworld[s];
            for (std::size_t a = 0; a < q_s.size(); ++a) {
                ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
            }
        }

        return String(ss.str().c_str());
    });
}

void LineWorldNode::launch_value_iteration(const int cells) {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }

    calculation_pending = true;

    current_calculation = std::async(std::launch::async, [cells]() {
        LineWorld lineworld(cells);
        auto value_function_lineworld = value_iteration(lineworld, 0.9f, 0.0001f);

        std::stringstream ss;
        for (std::size_t s = 0; s < value_function_lineworld.size(); ++s) {
            ss << "V(s=" << s << ") = " << value_function_lineworld[s] << std::endl;
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