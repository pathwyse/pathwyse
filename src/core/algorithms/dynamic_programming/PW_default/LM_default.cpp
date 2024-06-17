#include "LM_default.h"
#include <algorithm>

/** LM management **/
//Constructors and destructors
LMDefault::LMDefault(Problem* problem) {
    executionID = 0;
    name = "label_manager";
    lm_type = "pqueue";
    this->problem = problem;
    queue_limit = UNKNOWN;

    readConfiguration();

    collector = DataCollector(name);
    initDataCollection();

    best_fw = nullptr;
    best_bw = nullptr;
    iterations = 0;
}


//Init
void LMDefault::initLM(){
    //Get data from problem
    auto objective = problem->getObj();
    std::vector<Resource*>& resources = problem->getResources();

    int origin = problem->getOrigin();
    int destination = problem->getDestination();
    int n_nodes = problem->getNumNodes();
    int n_res = problem->getNumRes();

    //Reserve space for labels
    forward_labels.reserve(reserve_size);
    backward_labels.reserve(reserve_size);
    ndominated_fw = ndominated_bw = nclosed_fw = nclosed_bw = 0;

    //Data structures initialization
    forward_candidates.resize(n_nodes, std::list<std::pair<int, int>>());
    backward_candidates.resize(n_nodes, std::list<std::pair<int, int>>());
    forward_closed.resize(n_nodes, std::vector<std::pair<int, int>>());
    backward_closed.resize(n_nodes, std::vector<std::pair<int, int>>());
    forward_best.resize(n_nodes, nullptr);
    backward_best.resize(n_nodes, nullptr);

    //Push back first forward and backward labels
    bool direction = true;
    LabelAdv* predecessor = nullptr;

    //Initialize snapshots
    forward_labels.emplace_back();
    forward_labels[0].initLabel(origin, predecessor, direction, n_res);
    if(use_visited)
        forward_labels[0].initVisited(origin, n_nodes);
    forward_labels[0].setObjective(objective->getInitValue() + objective->getNodeCost(origin));

    if(bidirectional) {
        backward_labels.emplace_back();
        backward_labels[0].initLabel(destination, predecessor, !direction, n_res);
        if(use_visited)
            backward_labels[0].initVisited(destination, n_nodes);
        backward_labels[0].setObjective(objective->getNodeCost(destination));
    }

    for(int id = 0; id < problem->getNumRes(); id++) {
        forward_labels[0].setSnapshot(id, resources[id]->getInitValue() + resources[id]->getNodeCost(origin));
        if(bidirectional)
            backward_labels[0].setSnapshot(id, resources[id]->getInitValue() + resources[id]->getNodeCost(destination));
    }

    //Insert labels in the pool
    forward_candidates[origin].emplace_back(std::make_pair(forward_labels[0].getObjective(), 0));

    if(bidirectional)
        backward_candidates[destination].emplace_back(std::make_pair(backward_labels[0].getObjective(), 0));

    forward_best[origin] = & forward_labels[0];
    backward_best[destination] = & backward_labels[0];
    incumbent = UNKNOWN;
    turn_forward = origin;
    turn_backward = destination;
    od_label.second = -1;
    joinComparisons = 0;
}

void LMDefault::readConfiguration() {
    bidirectional = Parameters::isDefaultBidirectional();
    autoconfiguration = Parameters::isDefaultAutoConfigured();
    split_ratio = Parameters::getDefaultSplit();
    reserve_size = Parameters::getDefaultReserve();
    compare_unreachables = Parameters::isDefaultUsingUnreachables();
    use_visited = compare_unreachables or Parameters::isDefaultUsingVisited();
    candidate_type = Parameters::getDefaultCandidateType();
    join_type = Parameters::getDefaultJoinType();

    if(autoconfiguration) {
        //Simple auto-tune of some parameters
        bool active = false;
        candidate_type = CANDIDATE_RR;

        if(problem->isGraphCyclic()){
            active = true;
            candidate_type = CANDIDATE_NODE;
        }

        compare_unreachables = use_visited = active;
    }
}

void LMDefault::resetLM(){
    forward_labels.clear();
    forward_candidates.clear();
    forward_closed.clear();
    forward_closed_backup.clear();
    forward_best.clear();
    forward_top_candidates.clear();

    backward_labels.clear();
    backward_candidates.clear();
    backward_closed.clear();
    backward_closed_backup.clear();
    backward_best.clear();
    backward_top_candidates.clear();

    joinable_labels.clear();
    best_fw = nullptr;
    best_bw = nullptr;
    incumbent = UNKNOWN;
}

void LMDefault::update_split(){
    if(Parameters::isCollecting())
        collector.collect("split", split_ratio);

    int f_size = forward_labels.size();
    int b_size = backward_labels.size();

    if(f_size - b_size + f_size*0.2 < 0)
        split_ratio += 0.05;
    else if(b_size - f_size + b_size*0.2 < 0)
        split_ratio -= 0.05;
}

/** Candidate management **/
//Returns true if an open label is available
bool LMDefault::candidatesAvailable(bool forward, bool backward) {
    for(int i = 0; i < forward_candidates.size(); i++) {
        if(forward and !forward_candidates[i].empty())
            return true;
        if(backward and !backward_candidates[i].empty())
            return true;
    }
    return false;
}

//Returns an open candidate
LabelAdv* LMDefault::getCandidate(bool forward, bool backward) {

    LabelAdv* candidate;

    switch(candidate_type){
        default:
            candidate = getCandidateRR(forward, backward);
            break;
        case CANDIDATE_NODE:
            candidate = getCandidateNode(forward, backward);
            break;
    }

    if(candidate != nullptr)
        candidate->getDirection() ? nclosed_fw++ : nclosed_bw++;

    return candidate;
}

//Returns an open candidate in a round-robin fashion
//(eg: min cost open label comes from node 1. Next candidate comes from node 2, then 3 and so on.)
LabelAdv* LMDefault::getCandidateRR(bool forward, bool backward){
    LabelAdv* candidate = nullptr;
    int index, position;
    bool direction;
    int score = UNKNOWN;
    bool foundLabel = false;

    int turn;
    if(forward) turn = turn_forward;
    else turn = turn_backward;

    while(!foundLabel) {

        if(turn >= forward_candidates.size()) turn = 0;

        if(forward and !forward_candidates[turn].empty()){
            score = forward_candidates[turn].begin()->first;
            index = forward_candidates[turn].begin()->second;
            direction = true;
            foundLabel = true;
        }

        if(backward and !backward_candidates[turn].empty() and backward_top_candidates.begin()->first < score) {
            score = backward_candidates[turn].begin()->first;
            index = backward_candidates[turn].begin()->second;
            direction = false;
            foundLabel = true;
        }
        position = turn;
        turn++;
    }

    if(direction) {
        candidate = & forward_labels[index];
        forward_closed[position].emplace_back(std::make_pair(score,index));
        forward_candidates[position].pop_front();
    }
    else {
        candidate = & backward_labels[index];
        backward_closed[position].emplace_back(std::make_pair(score,index));
        backward_candidates[position].pop_front();
    }

    if(forward) turn_forward = turn;
    if(backward) turn_backward = turn;

    return candidate;

}

//Returns an open label from a certain node until it has no open labels left
LabelAdv* LMDefault::getCandidateNode(bool forward, bool backward) {

    LabelAdv* candidate = nullptr;
    int index, position;
    bool direction;
    int score = UNKNOWN;
    bool foundLabel = false;

    int turn = forward? turn_forward : turn_backward;

    while(!foundLabel) {
        if(forward and backward) {
            if(forward_candidates[turn].empty() and backward_candidates[turn].empty())
                turn++;
        }
        else if((forward and forward_candidates[turn].empty()) or (backward and backward_candidates[turn].empty()))
            turn++;

        if(turn >= forward_candidates.size()) turn = 0;

        if(forward and !forward_candidates[turn].empty()){
            score = forward_candidates[turn].begin()->first;
            index = forward_candidates[turn].begin()->second;
            direction = true;
            foundLabel = true;
        }
        if(backward and !backward_candidates[turn].empty() and backward_top_candidates.begin()->first < score) {
            score = backward_candidates[turn].begin()->first;
            index = backward_candidates[turn].begin()->second;
            direction = false;
            foundLabel = true;
        }
    }

    if(direction) {
        candidate = & forward_labels[index];
        position = candidate->getNode();
        forward_closed[position].emplace_back(std::make_pair(score,index));
        forward_candidates[position].pop_front();
    }
    else {
        candidate = & backward_labels[index];
        position = candidate->getNode();
        backward_closed[position].emplace_back(std::make_pair(score,index));
        backward_candidates[position].pop_front();
    }

    if(forward) turn_forward = turn;
    if(backward) turn_backward = turn;

    return candidate;

}

/** Label management **/
//Extension
bool LMDefault::isNodeReachable(LabelAdv *label, int next_node){
    if(use_visited)
        return label->isReachable(next_node);
    else
        return isExtensionFeasible(label, next_node);
}

bool LMDefault::isExtensionFeasible(LabelAdv *label, int next_node) {
    int current_value;
    bool direction = label->getDirection();
    double bounding = 1;

    std::vector<Resource*>& resources = problem->getResources();

    int i = direction ? label->getNode() : next_node;
    int j = direction ? next_node : label->getNode();

    for(int id = 0; id < resources.size(); id++) {
        current_value = label->getSnapshot(id);
        current_value = resources[id]->extend(current_value, i, j, direction);
        if(!resources[id]->isFeasible(current_value, next_node, bounding, direction))
            return false;
    }

    return true;
}

bool LMDefault::isCriticalExtensionFeasible(LabelAdv *label, int next_node) {
    int current_value;
    bool direction = label->getDirection();
    double bounding = direction ? split_ratio : 1 - split_ratio;

    std::vector<Resource*>& resources = problem->getResources();

    int i = direction ? label->getNode() : next_node;
    int j = direction ? next_node : label->getNode();

    current_value = label->getSnapshot(RES_CRITICAL);

    if(direction)
        current_value = resources[RES_CRITICAL]->extend(current_value, i, j, direction);

    if(!resources[RES_CRITICAL]->isFeasible(current_value, next_node, bounding, direction))
        return false;

    return true;
}

void LMDefault::extendLabel(LabelAdv *current_label, LabelAdv *new_label, int next_node) {
    auto objective = problem->getObj();
    std::vector<Resource*>& resources = problem->getResources();
    *new_label = *current_label;
    new_label->updateLabel(next_node, current_label);
    bool direction = current_label->getDirection();

    int i = direction ? current_label->getNode() : next_node;
    int j = direction ? next_node : current_label->getNode();

    //Update Objective
    int current_value, new_value;
    current_value = current_label->getObjective();
    new_value = objective->extend(current_value, i, j, direction);
    new_label->setObjective(new_value);

    //Update resources
    for(int id = 0; id < problem->getNumRes(); id++) {
        current_value = current_label->getSnapshot(id);
        new_value = resources[id]->extend(current_value, i, j, direction);
        new_label->setSnapshot(id, new_value);
    }

}

void LMDefault::updateUnreachables(LabelAdv *candidate) {
    if(use_visited) {
        int node = candidate->getNode();
        bool direction = candidate->getDirection();
        std::vector<int> & neighbors = problem->getNeighbors(node, direction);
        for(auto & neigh: neighbors)
            if(not isExtensionFeasible(candidate, neigh))
                candidate->setUnreachable(neigh);
    }
}

//Insertion
//Objective based insert

LabelAdv* LMDefault::insert(LabelAdv* new_label) {

    //Checks if new label is suboptimal
    if(not problem->isGraphCyclic() and new_label->getObjective() > incumbent)
        return nullptr;

    const int origin = problem->getOrigin();
    const int destination = problem->getDestination();
    const int node = new_label->getNode();
    const bool direction = new_label->getDirection();

    if((direction and node == origin) or (not direction and node == destination))
        return nullptr;

    const int objective = new_label->getObjective();

    auto & labels = direction ? forward_labels : backward_labels;
    auto & candidates = direction ? forward_candidates : backward_candidates;
    auto & best = direction ? forward_best : backward_best;
    auto & closed = direction ? forward_closed : backward_closed;
    auto & ndominated = direction ? ndominated_fw : ndominated_bw;
    auto & nclosed = direction ? nclosed_fw : nclosed_bw;

    LabelAdv* old_label;

    //Heuristic) Check if the queue is full
    if(candidates[node].size() >= queue_limit) {
        auto & c = candidates[node].back();
        old_label = getLabel(direction, c.second);

        if (old_label->getObjective() <= objective or
            (old_label->getObjective() == objective and
                old_label->getSnapshot(RES_CRITICAL) <= new_label->getSnapshot(RES_CRITICAL)))
            return nullptr;

        candidates[node].pop_back();
        ndominated++;
    }

    //1) Closed labels dominate new label?
    for (auto & c: closed[node]) {
        old_label = getLabel(direction, c.second);
        if (dominates(old_label, new_label))
            return nullptr;
    }

    //2) Open labels (with better obj) dominate new label?
    auto c = candidates[node].begin();

    while (c != candidates[node].end() and c->first < objective) {
        old_label = getLabel(direction, c->second);
        if(dominates(old_label, new_label))
            return nullptr;
        ++c;
    }

    //3) Open labels (with equal obj) dominate new label? Are they dominated by new label?
    while (c != candidates[node].end() and c->first == objective) {
        old_label = getLabel(direction, c->second);
        if (dominates(old_label, new_label))
            return nullptr;
        if (dominates(new_label, old_label)) {
            c = candidates[node].erase(c);
            ndominated++;
        }
        else ++c;
    }

    //4) Insert label
    labels.push_back(*new_label);
    int position = labels.size() - 1;
    new_label = &labels[position];

    //Keep track of the lowest cost label at each node
    if(not best[node] or best[node]->getObjective() >= objective)
        best[node] = new_label;

    //Add it to either closed or open labels
    if ((direction and node == destination) or (not direction and node == origin)){
        closed[node].emplace_back(std::make_pair(objective, position));
        nclosed++;
    }
    else
        candidates[node].insert(c, std::make_pair(objective, position));

    //5) Are open labels (with worse obj) dominated by new label?
    while(c != candidates[node].end()){
        old_label = getLabel(direction, c->second);
        if(dominates(new_label, old_label)) {
            c = candidates[node].erase(c);
            ndominated++;
        }
        else ++c;
    }

    //Update incumbent
    if((node == origin or node == destination) and objective <= incumbent) {
        incumbent = objective;
        od_label.first = direction;
        od_label.second = position;
    }

    return new_label;
}

//Returns true if l1 dominates l2
bool LMDefault::dominates(LabelAdv* l1, LabelAdv* l2) const {

    if(l1->getObjective() > l2->getObjective())
        return false;

    for(int id = 0; id < l1->getSnapshot().size(); id++)
        if(l1->getSnapshot(id) > l2->getSnapshot(id))
            return false;

    //Unreachable dominance
    if(compare_unreachables)
        return ((l1->getUnreachable()) & ~(l2->getUnreachable())).none();

    return true;
}

/** Join **/
void LMDefault::join(){
    if(Parameters::getVerbosity() >= 4)
        std::cout<<"Joining..."<<std::endl;

    if(candidatesAvailable())
        closeLabels();

    switch(join_type){
        case JOIN_NAIVE:
            naiveJoin();
            break;
        case JOIN_CLASSIC:
            classicJoin();
            break;
        case JOIN_ORDERED:
            orderedJoin();
            break;
    }

    if(candidatesAvailable())
        restoreClosedLabels();
}


void LMDefault::closeLabels() {
    if(Parameters::getVerbosity() >= 0)
        std::cout<<"Closing open labels to perform join operations..."<<std::endl;

    forward_closed_backup = forward_closed;
    backward_closed_backup = backward_closed;
    for(int p = 0; p < forward_candidates.size(); p++){
        if(not forward_candidates[p].empty())
            for(auto & f : forward_candidates[p]) {
                forward_closed[p].push_back(f);
                nclosed_fw++;
            }
        if(not backward_candidates[p].empty())
            for(auto & b: backward_candidates[p]) {
                backward_closed[p].push_back(b);
                nclosed_bw++;
            }
    }
}

void LMDefault::restoreClosedLabels(){
    forward_closed = forward_closed_backup;
    backward_closed = backward_closed_backup;
}

void LMDefault::naiveJoin(){
    auto objective = problem->getObj();
    int cost;
    LabelAdv *label_forward, *label_backward;
    joinComparisons = 0;

    for (int i = 0; i < forward_closed.size(); i++)
        if(i != problem->getDestination())
            for(int j = 0; j < backward_closed.size(); j++)
                if(problem->areNeighbors(i, j, true) and j != problem->getOrigin()) {
                    for (auto &forward_data: forward_closed[i]){
                        label_forward = &forward_labels[forward_data.second];
                        for (auto &backward_data: backward_closed[j]) {
                            joinComparisons++;
                            label_backward = &backward_labels[backward_data.second];
                            cost = objective->join(label_forward->getObjective(), label_backward->getObjective(), i, j);
                            if(cost <= incumbent and
                               isJoinFeasible(label_forward, label_backward)) {
                                incumbent = cost;
                                joinable_labels.insert(std::make_tuple(cost, label_forward, label_backward));
                            }
                        }
                    }
                }

}

void LMDefault::classicJoin() {
    auto objective = problem->getObj();
    int cost;
    LabelAdv *label_forward , *best_label_forward;
    LabelAdv *label_backward, *best_label_backward;
    joinComparisons = 0;

    for (int i = 0; i < forward_closed.size(); i++)
        if (forward_best[i] and i != problem->getDestination())
            for (int j = 0; j < backward_closed.size(); j++)
                if (problem->areNeighbors(i, j, true) and (backward_best[j]) and j != problem->getOrigin()) {
                    cost = objective->join(forward_best[i]->getObjective(), backward_best[j]->getObjective(), i, j);
                    //cost best f(i) + best b(j) > bound -> go to next node j
                    if (cost <= incumbent) {
                        for (auto &forward_data: forward_closed[i]) {
                            label_forward = &forward_labels[forward_data.second];
                            cost = objective->join(label_forward->getObjective(), backward_best[j]->getObjective(), i, j);
                            //cost f(i) + best b(j) > bound -> go to next label f(i)
                            if (cost <= incumbent) {
                                for (auto &backward_data: backward_closed[j]) {
                                    joinComparisons++;
                                    label_backward = &backward_labels[backward_data.second];
                                    cost = objective->join(label_forward->getObjective(), label_backward->getObjective(), i, j);
                                    if (cost <= incumbent and
                                        isJoinFeasible(label_forward, label_backward)) {
                                        incumbent = cost;
                                        joinable_labels.insert(std::make_tuple(cost, label_forward, label_backward));
                                    }
                                }
                            }
                        }
                    }
                }
}

void LMDefault::orderedJoin(){
    auto objective = problem->getObj();
    std::multiset<std::tuple<int, int, int>> orderedPairs;
    int cost;
    int i, j;

    //Sort closed
    for(int p = 0; p < forward_closed.size(); p++){
        if(not forward_closed[p].empty())
            std::sort(forward_closed[p].begin(), forward_closed[p].end());
        if(not backward_closed[p].empty())
            std::sort(backward_closed[p].begin(), backward_closed[p].end());
    }

    //Prepare data structure
    for(i = 0; i < forward_closed.size(); i++)
        if(!forward_closed[i].empty() and i != problem->getDestination())
            for(j = 0; j < backward_closed.size(); j++)
                if(problem->areNeighbors(i, j, true) and !backward_closed[j].empty() and i != j and j != problem->getOrigin()) {
                    cost = objective->join(forward_best[i]->getObjective(), backward_best[j]->getObjective(), i, j);
                    if(cost <= incumbent)
                        orderedPairs.insert(std::make_tuple(cost, i, j));
                }

    //attempt join
    joinComparisons = 0;
    LabelAdv *label_forward, *label_backward;
    while(!orderedPairs.empty()){
        cost = std::get<0>(*orderedPairs.begin());
        if(cost <= incumbent){
            i = std::get<1>(*orderedPairs.begin());
            j = std::get<2>(*orderedPairs.begin());
            orderedPairs.erase(orderedPairs.begin());
            for(auto & f: forward_closed[i]) {
                label_forward = &forward_labels[f.second];
                //If current label (i) +  best label of j is worse than bound, skip to next i-j combination
                if(objective->join(label_forward->getObjective(), backward_best[j]->getObjective(), i, j) <= incumbent) {
                    for (auto &b: backward_closed[j]){
                        label_backward = &backward_labels[b.second];
                        cost = objective->join(label_forward->getObjective(), label_backward->getObjective() , i, j);
                        if (cost <= incumbent){
                            joinComparisons++;
                            if(isJoinFeasible(label_forward, label_backward)) {
                                incumbent = cost;
                                joinable_labels.insert(std::make_tuple(cost, label_forward, label_backward));
                                break;
                            }
                        }
                    }
                }
                else break;
            }
        }
        else break;
    }
}

bool LMDefault::isJoinFeasible(LabelAdv* label_forward, LabelAdv* label_backward) {
    int i = label_forward->getNode();
    int j = label_backward->getNode();

    int current_value;
    int snapshot_forward, snapshot_backward;
    std::vector<Resource*>& resources = problem->getResources();

    for(int resID = 0; resID < problem->getNumRes(); resID++) {
        snapshot_forward = label_forward->getSnapshot(resID);
        snapshot_backward = label_backward->getSnapshot(resID);
        int current_value = resources[resID]->join(snapshot_forward, snapshot_backward, i, j);
        if(!resources[resID]->isFeasible(current_value))
            return false;
    }

    if(use_visited)
        return (label_forward->getVisited() & label_backward->getVisited()).none();

    return true;
}

/** Solution management +*/
//Return a solution
std::tuple<int, LabelAdv*, LabelAdv*> LMDefault::getSolutionLabels() {
    std::tuple<int, LabelAdv*, LabelAdv*> solution_data = {0, nullptr, nullptr};

    if(bidirectional and joinFound())
        solution_data = getBestJoin();
    else {
        LabelAdv* candidate = getODLabel();
        if(candidate){
            std::get<0>(solution_data) = candidate->getObjective();
            candidate->getDirection()? std::get<1>(solution_data) = candidate : std::get<2>(solution_data) = candidate;
        }
    }

    return solution_data;
}

void LMDefault::setODLabel() {
    int bound = UNKNOWN;
    int index = - 1;
    bool direction = true;

    for(auto & candidate: forward_closed[problem->getDestination()])
        if(forward_labels[candidate.second].getObjective() < bound) {
            bound = candidate.first;
            index = candidate.second;
        }
    for(auto & candidate: backward_closed[problem->getOrigin()])
        if(backward_labels[candidate.second].getObjective() < bound) {
            bound = candidate.first;
            index = candidate.second;
            direction = false;
        }

    if(index >= 0) {
        od_label.first = direction;
        od_label.second = index;
    }
}

/** Debug **/
//Finds labels with same obj and resource consumption
std::list<LabelAdv> LMDefault::buildTour(std::list<int> tour, bool direction) {
    if(!direction) tour.reverse();

    std::list<LabelAdv> tourLabels;

    tourLabels.emplace_back();
    LabelAdv* current_label = & tourLabels.front();

    int node = tour.front();
    tour.pop_front();

    current_label->initLabel(node, nullptr, direction, problem->getNumRes());
    current_label->setObjective(problem->getObj()->getInitValue() + problem->getObj()->getNodeCost(node));

    std::vector<Resource*>& resources = problem->getResources();
    for(int id = 0; id < problem->getNumRes(); id++)
        current_label->setSnapshot(id, resources[id]->getInitValue() + resources[id]->getNodeCost(node));

    for(auto node: tour) {
        LabelAdv new_label;
        extendLabel(current_label, &new_label, node);
        tourLabels.push_back(new_label);
        current_label = & tourLabels.back();
    }

    return tourLabels;
}

void LMDefault::findLabels(std::list<LabelAdv> *tourLabels) {
    if(Parameters::getVerbosity() < 0)
        return;

    int position;
    bool direction;
    bool found;
    LabelAdv* l_stored;
    std::vector<std::vector<std::pair<int, int>>> * stored_data;

    for(auto & l_current: *tourLabels) {
        position = l_current.getNode();
        direction = l_current.getDirection();
        found = false;

        if(direction) stored_data = & forward_closed;
        else stored_data = & backward_closed;

        for(auto & label_data: (*stored_data)[position]) {
            int index = std::get<1>(label_data);

            if(direction) l_stored = & forward_labels[index];
            else l_stored = & backward_labels[index];

            if(l_current.getObjective() == l_stored->getObjective()
               and l_current.getSnapshot() == l_stored->getSnapshot()){
                found = true;
                break;
            }

        }

        if(found)
            std::cout<<"Debug: Every label was found." << std::endl;
        else
            std::cout<<"Debug: Could not find matching label at node."  << position << std::endl;
    }
}

void LMDefault::printStepConsumption(std::list<LabelAdv> *tourFW, std::list<LabelAdv> *tourBW) {
    if(Parameters::getVerbosity() < 0)
        return;

    std::vector<std::vector<int>> consumptionsFW, consumptionsBW;
    consumptionsFW.resize(problem->getNumRes(), std::vector<int>());
    consumptionsBW.resize(problem->getNumRes(), std::vector<int>());

    for(auto & l: *tourFW)
        for(int r = 0; r < problem->getNumRes(); r++)
            consumptionsFW[r].push_back(l.getSnapshot(r));

    if(tourBW)
        for(auto & l: *tourBW)
            for(int r = 0; r < problem->getNumRes(); r++)
                consumptionsBW[r].push_back(l.getSnapshot(r));


    std::cout<<"Resource consumption at each node of the tour"<<std::endl;
    for(int r = 0; r < problem->getNumRes(); r++) {
        std::cout<<"Resource " << r << ": " << problem->getRes(r)->getName() << std::endl;
        std::cout<< "Direction: forward" << std::endl;
        for(int t = 0; t < tourFW->size(); t++)
            std::cout<< consumptionsFW[r][t]<< " ";
        std::cout<<std::endl;

        if(tourBW) {
            std::cout<< "Direction: backward" << std::endl;
            for(int t = 0; t < tourBW->size(); t++)
                std::cout<<consumptionsBW[r][t]<< " ";
            std::cout<<std::endl;
        }

        std::cout<<"-------"<<std::endl;
    }

}

void LMDefault::printCandidates(int id, bool direction) {
    if(Parameters::getVerbosity() < 0)
        return;

    std::cout<<"Printing all candidate labels for node " << id << " with direction " << direction << std::endl;

    LabelAdv* label;
    auto candidates = direction ? forward_candidates[id] : backward_candidates[id];
    for(auto c: candidates)
        direction ? forward_labels[c.second].printLabel() : backward_labels[c.second].printLabel();
}

void LMDefault::printClosed(int id, bool direction) {
    if(Parameters::getVerbosity() < 0)
        return;

    std::cout<<"Printing all closed labels for node " << id << " with direction " << direction << std::endl;

    LabelAdv* label;
    auto closed = direction ? forward_closed[id] : backward_closed [id];

    for(auto c: closed)
        direction ? forward_labels[c.second].printLabel() : backward_labels[c.second].printLabel();
}

/** Data collection management **/

void LMDefault::initDataCollection() {
    if(not Parameters::isCollecting())
        return;

    collector.init("lm_name", name);
    collector.init("data_structure", lm_type);
    collector.init("executionID", 0);
    collector.init("iterations", 0);
    collector.init("nlabels", 0);
    collector.init("nfw", 0);
    collector.init("nbw", 0);
    collector.init("ndominated", 0);
    collector.init("ndominated_fw", 0);
    collector.init("ndominated_bw", 0);
    collector.init("nclosed", 0);
    collector.init("nclosed_fw", 0);
    collector.init("nclosed_bw", 0);
    collector.init("njoin", "0");
    collector.init("split", split_ratio);
    collector.setHeader();
}

float LMDefault::getMeanLabels(bool direction){
    float sum_size = 0;

    auto & closed_labels = direction ? forward_closed : backward_closed;
    for(auto & c: closed_labels)
        sum_size += c.size();

    return (sum_size/closed_labels.size());
}

int LMDefault::getMaxLabels(bool direction) {
    int max_size = 0;

    auto & closed_labels = direction ? forward_closed : backward_closed;
    for(auto & c: closed_labels)
        if(max_size < c.size())
            max_size = c.size();

    return max_size;
}

float LMDefault::getVarLabels(bool direction, float mean) {
    float sum_square = 0;

    auto & closed_labels = direction ? forward_closed : backward_closed;
    for(auto & c: closed_labels)
        sum_square += (mean - c.size())*(mean - c.size());

    return (sum_square/closed_labels.size());
}

void LMDefault::collectData() {
    if(not Parameters::isCollecting())
        return;

    collector.collect("lm_name", name);
    collector.collect("executionID", executionID);
    collector.collect("iterations", iterations);
    collector.collect("nlabels", (int) (forward_labels.size() + backward_labels.size()));
    collector.collect("nfw", (int) forward_labels.size());
    collector.collect("nbw", (int) backward_labels.size());
    collector.collect("ndominated", ndominated_fw + ndominated_bw);
    collector.collect("ndominated_fw", ndominated_fw);
    collector.collect("ndominated_bw", ndominated_bw);
    collector.collect("nclosed", nclosed_fw + nclosed_bw);
    collector.collect("nclosed_fw", nclosed_fw);
    collector.collect("nclosed_bw", nclosed_bw);
    collector.collect("njoin", std::to_string(joinComparisons));
    collector.saveRecord();
}



