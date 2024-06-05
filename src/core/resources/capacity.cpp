#include "capacity.h"

Capacity::Capacity() {
    name = "Capacity";
    init_value = 0;
}

int Capacity::extend(int current_value, int i, int j, bool direction) {
    int dest_node = direction ? j : i;
    return current_value + data->getNodeCost(dest_node);
}

int Capacity::join(int current_value_forward, int current_value_backward, int i, int j){
    return current_value_forward + current_value_backward;
}

int Capacity::join(int current_value_forward, int current_value_backward, int node){
    return current_value_forward + current_value_backward - data->getNodeCost(node);
}

bool Capacity::isFeasible(int current_value, int current_node, double bounding, bool direction) {
    return current_value <= upper_bound*bounding;
}