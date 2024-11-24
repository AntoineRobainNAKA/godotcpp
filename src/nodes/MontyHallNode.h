#pragma once

#include "environments/2RoundRPS.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <memory>
#include <future>

namespace godot {
    
    class MontyHallNode : public Node {
        GDCLASS(MontyHallNode, Node);

    private:
        std::future<String> current_calculation;
        bool calculation_pending;
        String cached_result;

    protected:
        static void _bind_methods();

    public:
        MontyHallNode();
        ~MontyHallNode();

        void launch_algorithm(int algorithm_type);
        bool is_calculation_complete() const;
        String get_result();

        void _process(double delta) override;
    };
}
