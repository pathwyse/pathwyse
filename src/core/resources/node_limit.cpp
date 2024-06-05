#include "node_limit.h"

NodeLim::NodeLim() {
    name = "Node Limit";
    init_value = 1;
}

int NodeLim::extend(int current_value, int i, int j, bool direction) {
    return current_value + 1;
}

int NodeLim::join(int current_value_forward, int current_value_backward, int i, int j){
    return current_value_forward + current_value_backward;
}

int NodeLim::join(int current_value_forward, int current_value_backward, int node){
    return current_value_forward + current_value_backward - 1;
}

bool NodeLim::isFeasible(int current_value, int current_node, double bounding, bool direction) {
    return current_value <= upper_bound*bounding;
}