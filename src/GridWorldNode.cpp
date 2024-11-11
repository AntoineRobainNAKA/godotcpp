#include "GridWorldNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void GridWorldNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("launch_policy_iteration"), &GridWorldNode::launch_policy_iteration);
}

GridWorldNode::GridWorldNode() {
    rows = 5;
    columns = 5;
    //gridworld = GridWorld<rows, columns>;
}

GridWorldNode::~GridWorldNode() {
    rows = 0;
    columns = 0;
}

// ps : le résultat retourné n'est pas bon (c'est évident si on regarde)
// C'est un problème dans le gridworld directement
// pas encore trouvé la solution ¯\_(ツ)_/¯
String GridWorldNode::launch_policy_iteration(const int rows, const int columns) {
    // Pas trouvé le moyen de passer la taille en paramètre
    // C'est une connerie avec le template qui existe des const
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
}

// Obligé de mettre _process sinon ça compile pas
void GridWorldNode::_process(double delta) {
    //nothing for now I guess
}