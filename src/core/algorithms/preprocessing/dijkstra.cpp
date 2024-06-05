#include "algorithms/preprocessing/dijkstra.h"

/** Algorithm management **/
//Constructors and destructors
Dijkstra::Dijkstra(std::string name, Problem* problem): Algorithm(name, problem) {
    visited = Bitset(problem->getNumNodes());
    distances.resize(problem->getNumNodes(), UNKNOWN);
    lower_bound = INFMINUS;
    obj = problem->getObj();
    res = problem->getRes(RES_CRITICAL);
    found_optimal = false;
    round = 0;

    //Init incumbent solution
    std::get<0>(solution_data) = UNKNOWN;
    std::get<1>(solution_data) = nullptr;
    std::get<2>(solution_data) = nullptr;
    setStatus(ALGO_READY);
}

//Initialize algorithm
void Dijkstra::initAlgorithm(bool direction, int res_id, double bounding) {
    bound_labels = problem->getBoundLabels();
    this->direction = direction;
    this->res_id = res_id;
    this->bounding = bounding;
    origin = direction ? problem->getOrigin() : problem->getDestination();
    destination = direction ? problem->getDestination() : problem->getOrigin();
    labels = res_id == RES_COST ? bound_labels->getCostLabels(direction) : bound_labels->getConsumptionLabels(res_id, direction);
}

//Solve procedure
void Dijkstra::solve() {
    auto start_t = std::chrono::system_clock::now();
    setStatus(ALGO_OPTIMIZING);

    int cost;
    int current_value, new_value;

    Label new_label;

    //Initialization
    int current = origin;
    Label* current_label = & labels->at(current);
    current_label->initLabel(origin, nullptr, direction, 1);
    current_label->setObjective(obj->getInitValue() + obj->getNodeCost(origin));
    current_label->setSnapshot(RES_CRITICAL, res->getInitValue() + res->getNodeCost(origin));

    distances[current] = getDistance(current_label) + problem->getCoordDistance(current, destination);
    pq.emplace(distances[current], current);
    while (not pq.empty()) {
        current = pq.top().second;

        if (visited.get(current)) {
            pq.pop();
            continue;
        }
        visited.set(current);
        current_label = &labels->at(current);
        cost = pq.top().first;

        if (not isLabelValid(current_label, cost))
            break;

        pq.pop();
        if(round > 0)
            joinCompletion(current_label);

        if (not isCriticalLabelValid(current_label, cost))
            continue;

        auto &neighbors = problem->getNeighbors(current, direction);
        for (auto &node: neighbors)
            if (problem->isActiveNode(node) and not visited.get(node)) {
                new_label = *current_label;
                new_label.updateLabel(node, current_label);

                int i = direction ? current_label->getNode() : node;
                int j = direction ? node : current_label->getNode();

                //Update Objective
                current_value = current_label->getObjective();
                new_value = obj->extend(current_value, i, j, direction);
                new_label.setObjective(new_value);

                if(res_id == RES_COST) {
                    cost = getDistance(&new_label) + problem->getCoordDistance(node, destination);
                    if(distances[node] <= cost)
                        continue;
                }

                //Update res
                current_value = current_label->getSnapshot(RES_CRITICAL);
                new_value = res->extend(current_value, i, j, direction);
                new_label.setSnapshot(RES_CRITICAL, new_value);

                if(res_id != RES_COST) {
                    cost = getDistance(&new_label) + problem->getCoordDistance(node, destination);
                    if(distances[node] <= cost)
                        continue;
                }

                if (isLabelValid(current_label, cost)) {
                    distances[node] = cost;
                    labels->at(node) = new_label;
                    pq.emplace(distances[node], node);
                }
            }
    }

    checkFeasibility();
    checkOptimality();
    round++;

    if(Parameters::getVerbosity() >= 3) {
        auto end_t = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_t-start_t;
        std::cout << "Time: " << elapsed_seconds.count() << std::endl;

        if(res_id == RES_COST)
            std::cout<<"Min cost: " << distances[destination]<<std::endl;
        else
         std::cout<<"Min consumption: " << distances[destination]<<std::endl;
    }

    setStatus(ALGO_DONE);
}


//Check if a Label is still valid
bool Dijkstra::isLabelValid(Label* l, int cost) {
    if(res_id == RES_COST)
        return cost <= incumbent;
    else
        return res->isFeasible(cost, l->getNode(), 1, l->getDirection());
}

//Stand-alone method for extension
void Dijkstra::extendLabel(Label* current_label, Label & new_label, int next_node) {
    new_label = *current_label;
    new_label.updateLabel(next_node, current_label);

    int i = direction ? current_label->getNode() : next_node;
    int j = direction ? next_node : current_label->getNode();

    //Update Values
    int current_value, new_value;

    //Update Objective
    current_value = current_label->getObjective();
    new_value = obj->extend(current_value, i, j, direction);
    new_label.setObjective(new_value);

    //Update res
    current_value = current_label->getSnapshot(RES_CRITICAL);
    new_value = res->extend(current_value, i, j, direction);
    new_label.setSnapshot(RES_CRITICAL, new_value);
}

//Check if a label is still valid for some budget allocation
bool Dijkstra::isCriticalLabelValid(Label* l, int cost) {
    if(res_id == RES_CRITICAL)
        return res->isFeasible(l->getSnapshot(RES_CRITICAL), l->getNode(), bounding, l->getDirection());

    return true;
}

void Dijkstra::resetAlgorithm(int reset_level) {
    visited.reset();
    std::fill(distances.begin(), distances.end(), UNKNOWN);
    setStatus(ALGO_READY);
}


//Attempts join with completion labels
void Dijkstra::joinCompletion(Label* current){
    int node = current->getNode();
    bool terminate_join = false;

    bool direction_opp = not current->getDirection();
    Label* completion_label = bound_labels->getLabel(RES_COST, direction_opp, node);
    if(completion_label->getObjective() < incumbent)
        terminate_join = joinLabels(current, completion_label);

    if(not terminate_join) {
        completion_label = bound_labels->getLabel(RES_CRITICAL, direction_opp, node);
        if(completion_label->getObjective() < incumbent)
            terminate_join = joinLabels(current, completion_label);
    }
}

//Joins two labels
bool Dijkstra::joinLabels(Label* l1, Label* l2){
    int node = l1->getNode();
    int cost, consumption;

    if(direction)
        cost = obj->join(l1->getObjective(), l2->getObjective(), node);
    else
        cost = obj->join(l2->getObjective(), l1->getObjective(), node);

    if(cost >= incumbent)
        return true;

    if(direction)
        consumption = res->join(l1->getSnapshot(RES_CRITICAL), l2->getSnapshot(RES_CRITICAL), node);
    else
        consumption = res->join(l2->getSnapshot(RES_CRITICAL), l1->getSnapshot(RES_CRITICAL), node);

    if(res->isFeasible(consumption, node)) {
        incumbent = cost;
        std::get<0>(solution_data) = incumbent;
        std::get<1>(solution_data) = l1;
        std::get<2>(solution_data) = l2;
        return true;
    }

    return false;
}

//Checks for feasibility and sets problem status
void Dijkstra::checkFeasibility(){
    if(round == 0 and Parameters::isPreprocessingCritical())
        return;

    //If status has already been set, return
    if(problem->getStatus() != PROBLEM_INDETERMINATE)
        return;

    //If a feasible solution was found, the problem is feasible
    if(incumbent != UNKNOWN) {
        problem->setStatus(PROBLEM_FEASIBLE);
        return;
    }

    if(res_id == RES_CRITICAL) {
        if(distances[destination] != UNKNOWN){
            if(problem->getNumRes() == 1)
                problem->setStatus(PROBLEM_FEASIBLE);
            Label* l = bound_labels->getLabel(res_id, direction, destination);
            if(updateIncumbent(l->getObjective())){
                std::get<0>(solution_data) = incumbent;
                std::get<1>(solution_data) = l;
            }
        }
        else problem->setStatus(PROBLEM_INFEASIBLE);
    }
}

//Checks for optimality
void Dijkstra::checkOptimality(){
    if(res_id == RES_COST and distances[destination] != UNKNOWN)
        lower_bound = distances[destination];

    if(lower_bound == incumbent)
        found_optimal = true;
}

/** Solution management **/
//Builds a path
void Dijkstra::managePaths() {

    int objective = std::get<0>(solution_data);
    Label* fw = std::get<1>(solution_data);
    Label* bw = std::get<2>(solution_data);
    buildPath(objective, fw, bw);

    auto bestPath = getBestSolution();
    if(not bestPath)
        return;

    bestPath->setStatus(PATH_OPTIMAL);

    std::list<Label> tourFW = buildTour(bestPath->getTour(), true);
    bestPath->setConsumption(tourFW.back().getSnapshot());
}

//Builds a tour
std::list<Label> Dijkstra::buildTour(std::list<int> tour, bool direction) {
    if(!direction) tour.reverse();

    std::list<Label> tourLabels;

    tourLabels.emplace_back();
    Label* current_label = & tourLabels.front();

    int node = tour.front();
    tour.pop_front();

    current_label->initLabel(node, nullptr, direction, problem->getNumRes());
    current_label->setObjective(problem->getObj()->getInitValue() + problem->getObj()->getNodeCost(node));

    std::vector<Resource*>& resources = problem->getResources();
    for(int id = 0; id < problem->getNumRes(); id++)
        current_label->setSnapshot(id, resources[id]->getInitValue() + resources[id]->getNodeCost(node));

    for(auto node: tour) {
        Label new_label;
        extendLabel(current_label, new_label, node);
        tourLabels.push_back(new_label);
        current_label = & tourLabels.back();
    }

    return tourLabels;
}