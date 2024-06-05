#include "path.h"

/** Path management **/

Path::Path(const Path &obj) {
    solution_status = obj.solution_status;
    tour = obj.tour;
    objective = obj.objective;
    arc_cost = obj.arc_cost;
    node_cost = obj.node_cost;
    consumption = obj.consumption;
    labels = obj.labels;
    elementary = obj.elementary;
}

void Path::initPath() {
    solution_status = PATH_UNKNOWN;
    tour.clear();

    elementary = false;
    objective = UNKNOWN;
    arc_cost = UNKNOWN;
    node_cost = UNKNOWN;
    consumption.clear();
    labels.clear();
}

/** Tour management **/

std::string Path::getTourAsString() {
    std::string flat_tour;
    for(auto t: tour)
        flat_tour += std::to_string(t) + " ";
    return flat_tour;
}

void Path::setTour(std::list<int> tour) {
    this->tour = tour;
    elementary = checkElementarity();
}

//Check if a tour is elementary
bool Path::checkElementarity(){
    auto t = tour;
    t.sort();

    int prev_node = -1;
    for(auto current_node: t) {
        if(prev_node == current_node)
            return false;
        else
            prev_node = current_node;
    }
    return true;
}

/** Output management **/

void Path::printPath() {
    if(Parameters::getVerbosity() >= 0) {
        printStatus();
        std::cout<<"Obj: " << objective << std::endl;
    }
    if(Parameters::getVerbosity() >= 1) {
        std::cout << "Tour Length: " << tour.size() << std::endl;
        if(Parameters::getVerbosity() >= 2){
            std::cout << "Tour: ";
            for(auto& t: tour)
                std::cout << t << " ";
            std::cout<<std::endl;
        }
    }

    if(Parameters::getVerbosity() >= 2) {
        std::cout<<"Consumption: ";
        for(auto& c: consumption)
            std::cout << c << " ";
        std::cout<<std::endl;
    }

    if(Parameters::getVerbosity() >= 4)
        for(auto & l: labels) l.printLabel();
}

void Path::printStatus() {
    std::string status;

    switch(solution_status){
        case PATH_UNKNOWN: status = "Unknown"; break;
        case PATH_OPTIMAL: status = "Optimal"; break;
        case PATH_FEASIBLE: status = "Feasible"; break;
        case PATH_INFEASIBLE: status = "Infeasible"; break;
        case PATH_SUPEROPTIMAL: status = "Super optimal"; break;
        default: break;
    }

    std::cout << "Solution Status: " << status <<  std::endl;
}