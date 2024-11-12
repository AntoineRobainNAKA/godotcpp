#include "GridWorldNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void GridWorldNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_policy_iteration"), &GridWorldNode::launch_policy_iteration);
    ClassDB::bind_method(D_METHOD("launch_q_learning"), &GridWorldNode::launch_q_learning);
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

// ps : le résultat retourné n'est pas bon (c'est évident si on regarde)
// C'est un problème dans le gridworld directement
// pas encore trouvé la solution ¯\_(ツ)_/¯
void GridWorldNode::launch_policy_iteration(const int rows, const int columns) {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progres !");
        return;
    }    
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, []() {
        // Pas trouvé le moyen de passer la taille en paramètre
        // C'est une connerie avec le template qui exige des const
        GridWorld<5, 5> gridworld;
        auto [pi_gridworld, value_function_gridworld] = policy_iteration(gridworld, 0.999f, 0.001f);

        std::stringstream ss;

        for (std::size_t s = 0; s < pi_gridworld.size(); ++s) {
            ss << "Pi(s=" << s << ") = " << pi_gridworld[s] << std::endl;
        }
        for (std::size_t s = 0; s < value_function_gridworld.size(); ++s) {
            ss << "V(s=" << s << ") = " << value_function_gridworld[s] << std::endl;
        }

        // Godot veut la String (godot::String) et pas std::string
        return String(ss.str().c_str());
    });
}

void GridWorldNode::launch_q_learning() {
    if (calculation_pending) {
        UtilityFunctions::print("Calculation already in progress !");
        return;
    }    
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, []() {
        GridWorld<5,5> gridworld;
        auto q_values_gridworld = q_learning<GridWorld<5, 5>>(10000, 0.1f, 0.999f, 1.0f);
        std::stringstream ss;

        for (std::size_t s = 0; s < q_values_gridworld.size(); ++s) {
            const auto &q_s = q_values_gridworld[s];
            for (std::size_t a = 0; a < q_s.size(); ++a) {
                ss << "Q(s=" << s << ", a=" << a << ") = " << q_s[a] << std::endl;
            }
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