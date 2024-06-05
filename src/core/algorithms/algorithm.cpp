#include "algorithm.h"

/** Algorithm management **/
Algorithm::Algorithm(std::string name, Problem* problem){
    executionID = 0;
    algo_type = "NA";
    parallel = false;
    bidirectional = false;
    best_solution_id = -1;
    this->name = name;

    initAlgorithm();
    setProblem(problem);
    initDataCollection();
}

void Algorithm::initAlgorithm(){
    iterations = 0;
    setStatus(ALGO_READY);
    incumbent = INFPLUS;
    lower_bound = INFMINUS;
}

/** Solution management **/
void Algorithm::addSolution(Path & path) {
    solutions.push_back(path);
    auto bestPath = getBestSolution();
    if(not bestPath or path.getObjective() < getBestSolution()->getObjective())
        best_solution_id = solutions.size() - 1;
}

bool Algorithm::updateIncumbent(int value) {
    if(incumbent > value) {
        incumbent = value;
        return true;
    }
    return false;
}

bool Algorithm::updateLowerBound(int value) {
    if(lower_bound < value) {
        lower_bound = value;
        return true;
    }
    return false;
}

//Builds a path. NB: Algorithm cannot compute resource consumptions.
void Algorithm::buildPath(int objective, Label* forward_label, Label* backward_label){

    if(forward_label == nullptr and backward_label == nullptr)
        return;

    Path s;

    //Updates objective
    s.setObjective(objective);

    //Updates tour
    std::list<int> tour;

    while(forward_label != nullptr){
        tour.push_front(forward_label->getNode());
        forward_label = forward_label->getPredecessor();
    }

    while(backward_label != nullptr){
        tour.push_back(backward_label->getNode());
        backward_label = backward_label->getPredecessor();
    }

    s.setTour(tour);

    //Updates total cost of arcs and nodes
    if(s.isElementary()){
        int arcCost = 0;
        int nodeCost = 0;

        auto obj = problem->getObj();

        int i = problem->getOrigin();
        for(int j: tour) {
            //Get Node Cost
            if(i != problem->getDestination())
                nodeCost += obj->getNodeCost(i);

            //Get Arc cost
            if(i != j)
                arcCost += obj->getArcCost(i, j);

            i = j;
        }
        s.setArcCost(arcCost);
        s.setNodeCost(nodeCost);
    }

    addSolution(s);
}

/** Output management **/
void Algorithm::printStatus() {
    std::string status;

    switch(algo_status){
        case ALGO_READY:
            status = "Ready";
            break;
        case ALGO_OPTIMIZING:
            status = "Optimizing";
            break;
        case ALGO_DONE:
            status = "Optimization complete";
            break;
        case ALGO_TIMELIMIT:
            status = "Timelimit reached";
            break;
        case ALGO_BOUNDLIMIT:
            status = "Boundlimit reached";
            break;
        case ALGO_GAPLIMIT:
            status = "Gaplimit reached";
            break;
        default:
            break;
    }

    std::cout << name << " status: " << status <<  std::endl;
}

/** Data collection management **/
void Algorithm::initDataCollection() {
    collector = DataCollector("Algorithm");

    collector.initTime("time");

    if(not Parameters::isCollecting())
        return;

    collector.init("algo_name", name);
    collector.init("executionID", executionID);
    collector.init("iterations", iterations);
    collector.init("lb", lower_bound);
    collector.init("ub", incumbent);
    collector.init("algo_status", algo_status);
    collector.init("algo_type", algo_type);
    collector.init("bidirectional", bidirectional);
    collector.init("timeout", 0);

    collector_sol = DataCollector("Solutions");
    collector_sol.init("executionID", 0);
    collector_sol.init("iterations", iterations);
    collector_sol.init("elementary", 0);
    collector_sol.init("solution_status", PATH_UNKNOWN);
    collector_sol.init("objective", UNKNOWN);
    collector_sol.init("tour_length", -1);
    collector_sol.init("tour", "");
    for(int i = 0; i < problem->getNumRes(); i++)
        collector_sol.init("r" + std::to_string(i) + "_consumption", 0);
    collector_sol.setHeader();
}


void Algorithm::collectSolution(int id){
    if(not Parameters::isCollecting())
        return;

    collector_sol.collect("executionID", executionID);
    collector_sol.setCollectionName(name + "_sol");
    collector_sol.collect("iterations", iterations);

    if(solutions.empty() or id >= solutions.size()) {
        collector_sol.collect("elementary", 0);
        collector_sol.collect("objective", UNKNOWN);
        collector_sol.collect("tour_length", 0);
        collector_sol.collect("tour", "");
        for(int i = 0; i < problem->getNumRes(); i++)
            collector_sol.collect("r" + std::to_string(i) + "_consumption", 0);
    }
    else {
        auto & sol = solutions[id];

        collector_sol.collect("elementary", sol.isElementary());
        collector_sol.collect("solution_status", sol.getStatus());
        collector_sol.collect("objective", sol.getObjective());
        collector_sol.collect("tour_length", sol.getTourLength());
        collector_sol.collect("tour", sol.getTourAsString());
        for(int i = 0; i < problem->getNumRes(); i++)
            collector_sol.collect("r" + std::to_string(i) + "_consumption", sol.getConsumption(i));
    }

    collector_sol.saveRecord();
}