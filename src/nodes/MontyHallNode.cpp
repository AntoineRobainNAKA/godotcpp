#include "MontyHallNode.h"
#include "algorithms/PolicyIteration.h"
#include "algorithms/QLearning.h"
#include "algorithms/ValueIteration.h"
#include "algorithms/MonteCarloES.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sstream>

using namespace godot;

void MontyHallNode::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("launch_algorithm", "algorithm_type", "doors", "gamma", "theta", "num_episodes", "learning_rate", "epsilon"), &MontyHallNode::launch_algorithm);
    ClassDB::bind_method(D_METHOD("is_calculation_complete"), &MontyHallNode::is_calculation_complete);
    ClassDB::bind_method(D_METHOD("get_result"), &MontyHallNode::get_result);
}

MontyHallNode::MontyHallNode()
{
    calculation_pending = false;
}

MontyHallNode::~MontyHallNode()
{
    if (calculation_pending && current_calculation.valid())
    {
        current_calculation.wait();
    }
}

std::string get_state_description(size_t state_id, const MontyHallEnv &env) {
    size_t num_doors = env.num_actions();

    // State 0 is the initial state where no choice has been made yet
    if (state_id == 0) {
        return "Initial choice (all doors available)";
    }

    // Adjust state_id to calculate its components
    size_t adjusted_state = state_id - 1;

    // Determine if we are in the pre- or post-removal state space
    bool door_removed = (adjusted_state >= (num_doors * num_doors));

    if (door_removed) {
        // Post-removal state space
        adjusted_state -= (num_doors * num_doors); // Offset to start after pre-removal states
    }

    // Extract winning door and initial choice
    size_t winning_door = adjusted_state / num_doors;
    size_t initial_choice = adjusted_state % num_doors;

    std::stringstream ss;
    if (door_removed) {
        // Post-removal state
        size_t removed_door = env.get_removed_door();  // Fetch removed_door from the environment
        ss << "State after initial choice " << initial_choice
           << " (winning_door=" << winning_door
           << ", removed_door=" << removed_door << ")";
    } else {
        // Pre-removal state
        ss << "State after initial choice " << initial_choice
           << " (winning_door=" << winning_door
           << ", no door removed yet)";
    }

    return ss.str();
}


void MontyHallNode::launch_algorithm(int algorithm_type, int doors, float gamma, float theta, int num_episodes, float learning_rate, float epsilon)
{
    if (calculation_pending)
    {
        UtilityFunctions::print("Calculation already in progress!");
        return;
    }
    calculation_pending = true;

    current_calculation = std::async(std::launch::async, [this, algorithm_type, doors, gamma, theta, num_episodes, learning_rate, epsilon]()
                                     {
        MontyHallEnv mh;
        std::stringstream ss;

        switch (algorithm_type) {
            case 1: {
                ss << "Monty Hall is not a Markov Decision Process!\n";
                break;
            }
            case 2: {
                auto q_values_mh = q_learning(mh, num_episodes, learning_rate, gamma, epsilon);

                // Print Q-values for each state
                for (std::size_t s = 0; s < q_values_mh.size(); ++s)
                {
                    ss << "State " << s << " (" << get_state_description(s, mh) << "):\n";

                    // Find the best action for this state
                    float max_q = -std::numeric_limits<float>::infinity();
                    size_t best_action = 0;
                    for (std::size_t a = 0; a < q_values_mh[s].size(); ++a)
                    {
                        if (q_values_mh[s][a] > max_q)
                        {
                            max_q = q_values_mh[s][a];
                            best_action = a;
                        }
                    }

                    // Print Q-values for all actions, highlighting the best one
                    for (std::size_t a = 0; a < q_values_mh[s].size(); ++a)
                    {
                        ss << "  Door " << a << ": " << q_values_mh[s][a];
                        if (a == best_action)
                        {
                            ss << " (Best)";
                        }
                        ss << "\n";
                    }
                    ss << "\n";
                }

                // Add overall strategy summary
                ss << "\nOptimal Strategy Summary:\n";
                float initial_max_q = -std::numeric_limits<float>::infinity();
                size_t best_initial_choice = 0;
                for (size_t a = 0; a < q_values_mh[0].size(); ++a)
                {
                    if (q_values_mh[0][a] > initial_max_q)
                    {
                        initial_max_q = q_values_mh[0][a];
                        best_initial_choice = a;
                    }
                }
                ss << "Best initial choice: Door " << best_initial_choice << "\n";
                ss << "Strategy: ";
                if (initial_max_q > 0.5f)
                {
                    ss << "Always switch after initial choice\n";
                }
                else
                {
                    ss << "Stay with initial choice\n";
                }
            }
            break;
            case 3: {
                ss << "Monty Hall is not a Markov Decision Process!\n";
                break;
            }
            case 4: {
                auto q_values_mh = monte_carlo_es(mh, num_episodes, epsilon);
                
                ss << "Monte Carlo ES Results for Monty Hall:\n";
                for (std::size_t s = 0; s < q_values_mh.size(); ++s) {
                    ss << "State " << s << " (" << get_state_description(s, mh) << "):\n";
                    
                    float max_q = -std::numeric_limits<float>::infinity();
                    size_t best_action = 0;
                    for (std::size_t a = 0; a < q_values_mh[s].size(); ++a) {
                        if (q_values_mh[s][a] > max_q) {
                            max_q = q_values_mh[s][a];
                            best_action = a;
                        }
                    }

                    for (std::size_t a = 0; a < q_values_mh[s].size(); ++a) {
                        ss << "  Door " << a << ": " << q_values_mh[s][a];
                        if (a == best_action) {
                            ss << " (Best)";
                        }
                        ss << "\n";
                    }
                    ss << "\n";
                }
                break;
            }
            default:
                ss << "No algorithm selected";
                break;
        }

        return String(ss.str().c_str()); });
}

bool MontyHallNode::is_calculation_complete() const
{
    if (!calculation_pending)
        return false;
    if (!current_calculation.valid())
        return false;

    auto status = current_calculation.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
}

String MontyHallNode::get_result()
{
    if (!calculation_pending || !current_calculation.valid())
    {
        return String("No calculation in progress.");
    }

    cached_result = current_calculation.get();
    calculation_pending = false;
    return cached_result;
}

void MontyHallNode::_process(double delta)
{
    // Nothing needed here
}