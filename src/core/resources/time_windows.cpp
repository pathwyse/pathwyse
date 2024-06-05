#include "time_windows.h"
#include <algorithm>

TimeWindow::TimeWindow() {
    name = "Time Window";
    init_value = 0;
}

//Computes the upperbound.
//Represents the overall resource availability, i.e. the maximum feasible arrival time at destination.
void TimeWindow::init(int origin, int destination) {
    upper_bound = 0;

    for(int i = 0; i < node_upper_bound.size(); i++)
        if(i != destination)
            upper_bound = std::max(upper_bound, node_upper_bound[i] + data->getNodeCost(i) + data->getArcCost(i, destination));
}

int TimeWindow::extend(int current_value, int i, int j, bool direction) {
    int current_time = current_value + data->getArcCost(i, j);

    if(direction) {
        current_time += data->getNodeCost(i);
        current_time = std::max(current_time, node_lower_bound[j]); //fw: arrival time at j
    }
    else {
        current_time += data->getNodeCost(j);
        current_time = std::max(current_time, upper_bound - (node_upper_bound[i] + data->getNodeCost(i)));    //bw: time between departure from j to arrival at destination
    }

    return current_time;
}

int TimeWindow::join(int current_value_forward, int current_value_backward, int i, int j){
    return current_value_forward + data->getNodeCost(i) + data->getArcCost(i, j) + data->getNodeCost(j) + current_value_backward;
}

int TimeWindow::join(int current_value_forward, int current_value_backward, int node){
    return current_value_forward + current_value_backward - data->getNodeCost(node);
}

bool TimeWindow::isFeasible(int current_value, int current_node, double bounding, bool direction) {
    if(current_value > upper_bound*bounding) return false;

    if(current_node >= 0) {
        int feasible_value = direction ? node_upper_bound[current_node] : upper_bound - (node_lower_bound[current_node] + data->getNodeCost(current_node));
        if(current_value > feasible_value) return false;
    }

    return true;
}