#ifndef LINEWORLD
#define LINEWORLD

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <Environments/LineWorld.h>
#include <memory>
#include <future>

namespace godot {
    class LineWorldNode : public Node {
        GDCLASS(LineWorldNode, Node);

    private:
        int rows;
        int columns;
        std::future<String> current_calculation;
        bool calculation_pending;
        String cached_result;

    protected:
        static void _bind_methods();

    public:
        LineWorldNode();
        ~LineWorldNode();

        void launch_policy_iteration(const int cells);
        void launch_q_learning(const int cells);
        void launch_value_iteration(const int cells);
        bool is_calculation_complete() const;
        String get_result();

        void _process(double delta) override;
    };
}

#endif