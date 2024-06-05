#include "time.h"

Time::Time() {
    name = "Time";
    init_value = 0;
}

int Time::extend(int current_value, int i, int j, bool direction) {
    int dest_node = direction ? j : i;
    return current_value + data->getArcCost(i, j) + data->getNodeCost(dest_node);
}

int Time::join(int current_value_forward, int current_value_backward, int i, int j){
    return current_value_forward + current_value_backward + data->getArcCost(i, j);
}

int Time::join(int current_value_forward, int current_value_backward, int node){
    return current_value_forward + current_value_backward - data->getNodeCost(node);
}

bool Time::isFeasible(int current_value, int current_node, double bounding, bool direction) {
    return current_value <= upper_bound*bounding;
}