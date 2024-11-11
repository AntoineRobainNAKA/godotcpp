#ifndef GRIDWORLD
#define GRIDWORLD

#include <godot_cpp/classes/node.hpp>
#include <Environments/GridWorld.h>

namespace godot {
    class GridWorldNode : public Node {
        GDCLASS(GridWorldNode, Node);

    private:
        int rows;
        int columns;
        //GridWorld<1,1> gridworld;

    protected:
        static void _bind_methods();

    public:
        GridWorldNode();
        ~GridWorldNode();

        String launch_policy_iteration(const int rows, const int columns);
        void launch_q_learning();

        void _process(double delta) override;
    };
}

#endif