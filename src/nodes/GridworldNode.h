#ifndef GRIDWORLD
#define GRIDWORLD

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <Environments/GridWorld.h>
#include <memory>
#include <future>


namespace godot {
    class GridWorldNode : public Node {
        GDCLASS(GridWorldNode, Node);

    private:
        int rows;
        int columns;
        std::future<String> current_calculation;
        bool calculation_pending;
        String cached_result;

    protected:
        static void _bind_methods();

    public:
        GridWorldNode();
        ~GridWorldNode();

        void launch_policy_iteration(const int rows, const int columns);
        void launch_q_learning(const int rows, const int columns);
        void launch_value_iteration(const int rows, const int columns);
        bool is_calculation_complete() const;
        String get_result();

        void _process(double delta) override;
    };
}

#endif