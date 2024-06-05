#include "PW_default.h"
#include <thread>

/** Algorithm management **/
//Constructors and destructors

PWDefault::PWDefault(std::string name, Problem* problem): Algorithm(name, problem){
    label_manager = new LMDefault(problem);
    readConfiguration();
    initDataCollection();
    setStatus(ALGO_READY);
}

PWDefault::~PWDefault() {
    writeData();
    delete label_manager;
}

//Init and reset
void PWDefault::initAlgorithm() {
    //Data collection
    unreachable_max_count = 0;

    if(use_visited) {
        //Determines which nodes will be checked for unreachability. Each node has its own bitset. Default value: 0.
        unreachable_active.resize(problem->getNumNodes(), Bitset(problem->getNumNodes()));
        if (ng != NG_OFF or dssr != DSSR_OFF) {
            //If a relaxation is used, the relaxation technique decides which unreachable nodes will be checked
            if (ng != NG_OFF)
                //If NG-route relaxation is active, build ng-neighbourhood
                    buildNG();
        }
        else {
            //if no relaxation is used, all unreachable nodes will be checked
            for(auto & u: unreachable_active)
                u.set();
            unreachable_max_count = problem->getNumNodes();
        }
    }
}

void PWDefault::readConfiguration(){
    timelimit = Parameters::getDefaultTimelimit();
    parallel = Parameters::isDefaultParallel();
    bidirectional = Parameters::isDefaultBidirectional();
    dssr = Parameters::getDefaultDSSR();
    ng = Parameters::getDefaultNG();
    ng_size = Parameters::getDefaultNGSize();
    earlyjoin = Parameters::isDefaultJoinEarly();
    earlyjoin_step = Parameters::getDefaultJoinStep();
    use_visited = Parameters::isDefaultUsingVisited();
    algo_type = ALGO_EXACT;

    if(name == "PWDefaultRelaxDom") {
        dssr = DSSR_OFF;
        ng = NG_OFF;
        algo_type = ALGO_HEURISTIC;
        label_manager->setCompareUnreachables(false);
    }
    else if (name == "PWDefaultRelaxQueue") {
        dssr = DSSR_OFF;
        ng = NG_OFF;
        algo_type = ALGO_HEURISTIC;
        label_manager->setQueueLimit(Parameters::getDefaultRelaxationQueueLimit());
    }
}

void PWDefault::resetIteration(){
    //Reset data at each iteration
    iterations++;
    collector.resetTimes();
    previous_unreachable_max_count = unreachable_max_count;
    it_ext_fw = it_ext_bw = ins_attempts_fw = ins_attempts_bw = 0;
    label_manager->initLM();
    label_manager->setExecutionID(executionID);
    label_manager->setIteration(iterations);
}

void PWDefault::resetAlgorithm(int reset_level) {
    //Initializes bounds
    Algorithm::initAlgorithm();
    setStatus(ALGO_READY);

    //Resets configuration
    unreachable_active.clear();
    unreachable_ng.clear();
    readConfiguration();

    //Resets solutions
    best_solution_id = -1;
    solutions.clear();

    //Resets LM
    label_manager->resetLM();

    //Resets data collection and extra parameters
    collector.resetTimesCumulative();
    timeout = false;
}

//Solve problem
void PWDefault::solve(){
    if(Parameters::getVerbosity() >= 4)
        std::cout<<"Solving..."<<std::endl;

    setStatus(ALGO_OPTIMIZING);
    initAlgorithm();

    bool termination = false;
    while(not termination) {
        collector.startGlobalTime();
        resetIteration();

        //Labeling
        collector.startTime("t_labeling");
        if (parallel and bidirectional) {
            std::thread fw(&PWDefault::labeling, this, true, false);    //Forward labeling
            std::thread bw(&PWDefault::labeling, this, false, true);    //Backward labeling
            fw.join();
            bw.join();
        }
        else labeling();
        collector.stopTime("t_labeling");

        //If bidirectional search is over, join labels
        if (bidirectional) {
            label_manager->update_split();                              //Dynamic critical resource budget update

            //Join procedure
            collector.startTime("t_join");
            label_manager->join();
            collector.stopTime("t_join");
        }

        //Manage paths
        managePaths();
        label_manager->collectData();

        //Check Termination
        termination = checkTermination();

        collector.stopGlobalTime();
        collectData();
    }

    if(Parameters::getVerbosity() >= 3)
        std::cout<<"Solving complete"<<std::endl;

    if(algo_status == ALGO_OPTIMIZING)
        setStatus(ALGO_DONE);

    collector.print();
}


void PWDefault::labeling(bool forward, bool backward) {
    //If candidates are available
    while(label_manager->candidatesAvailable(forward, backward)) {
        //Get candidate label from label manager
        LabelAdv *candidate = label_manager->getCandidate(forward, backward);

        //Extend label towards reachable nodes
        collector.startTime(candidate->getDirection() ? "t_ext_fw" : "t_ext_bw");
        extend(candidate);
        collector.stopTime(candidate->getDirection() ? "t_ext_fw" : "t_ext_bw");
        candidate->getDirection() ? it_ext_fw++ : it_ext_bw++;

        //Early Join
        if(forward and backward and bidirectional and earlyjoin and
                earlyjoin_step <= label_manager->totalLabels()) {
            if(Parameters::getVerbosity() >= 2)
                std::cout << "Early join" << std::endl;

            if(earlyjoin_step < MAX_JOIN_STEP)
                earlyjoin_step *= earlyjoin_step;
            else
                earlyjoin = false;

            collector.startTime("t_join");
            label_manager->join();
            collector.stopTime("t_join");
        }
    }
}


void PWDefault::extend(LabelAdv* candidate) {
    LabelAdv new_label = LabelAdv();
    int node = candidate->getNode();
    std::vector<int> neighbors = problem->getNeighbors(node, candidate->getDirection());
    bool active;

    for(auto & neigh: neighbors) {

        active = not unreachable_active.empty() and unreachable_active[node].get(neigh);

        if((active and label_manager->isNodeReachable(candidate, neigh)) or
           (not active and label_manager->isExtensionFeasible(candidate, neigh))){

            //Eliminates k = 2 cycles
            if(candidate->getPredecessor() and candidate->getPredecessor()->getNode() == neigh)
                continue;

            //critical res extension check
            if(bidirectional and not label_manager->isCriticalExtensionFeasible(candidate, neigh))
                continue;

            //For each reachable node, extend label in that direction
            label_manager->extendLabel(candidate, &new_label, neigh);

            //Update unreachables
            label_manager->updateUnreachables(&new_label);
            if(not unreachable_active.empty())
                new_label.updateUnreachables(unreachable_active[neigh]);

            if(Parameters::getCollectionLevel()>=1)
                collector.startTime(candidate->getDirection() ? T_INS_FW : T_INS_BW);
            label_manager->insert(&new_label);
            if(Parameters::getCollectionLevel()>=1)
                collector.stopTime(candidate->getDirection() ? T_INS_FW : T_INS_BW);
            candidate->getDirection() ? ins_attempts_fw++ : ins_attempts_bw++;
        }
    }
}

bool PWDefault::checkTermination() {

    bool termination = true;
    auto bestPath = getBestSolution();

    //Condition 1: Timeout reached
    if(timeout) {
        if(bestPath and bestPath->isElementary())
            updateIncumbent(bestPath->getObjective());

        return termination;
    }

    //Condition 2: No solution found
    if(not bestPath) {
        if(algo_type == ALGO_EXACT)
            problem->setStatus(PROBLEM_INFEASIBLE);
        return termination;
    }

    if(algo_type == ALGO_EXACT)
        updateLowerBound(bestPath->getObjective());

    //Condition 3: Elementary solution found
    if(bestPath->isElementary()){
        updateIncumbent(bestPath->getObjective());

        if(ng != NG_OFF)
            ng_compliant = true;
        return termination;
    }

    //Condition 4: NG solution found (if DSSR is OFF)
    if(ng == NG_RESTRICTED) {
        termination = NgRestricted();
        ng_compliant = termination;
    }

    //If DSSR is enabled, termination will be false until an elementary solution is found
    if(dssr != DSSR_OFF and termination) {
        //NG algorithms are not used anymore
        ng = NG_OFF;
        termination = dssr == DSSR_STANDARD ? DssrStandard() : DssrRestricted();
    }

    //Reset for next iteration
    if(not termination) {
        label_manager->resetLM();
        solutions.clear();
    }

    return termination;
}

void PWDefault::managePaths(){
    if(timelimit > EPS and collector.getGlobalTime() > timelimit) {
        setStatus(ALGO_TIMELIMIT);
        timeout = true;
    }
    auto solution_data = label_manager->getSolutionLabels();

    int objective = std::get<0>(solution_data);
    auto *fw = std::get<1>(solution_data);
    auto *bw = std::get<2>(solution_data);
    buildPath(objective, fw, bw);

    auto bestPath = getBestSolution();

    if(not bestPath)
        return;

    if(bestPath->isElementary()){
        problem->setStatus(PROBLEM_FEASIBLE);
        if(algo_type == ALGO_EXACT and not timeout)
            bestPath->setStatus(PATH_OPTIMAL);
        else
            bestPath->setStatus(PATH_FEASIBLE);
    }
    else
        bestPath->setStatus(PATH_SUPEROPTIMAL);


    std::list<LabelAdv> tourFW = label_manager->buildTour(bestPath->getTour(), true);
    bestPath->setConsumption(tourFW.back().getSnapshot());
    collectSolution(best_solution_id);
}

/** Relaxation management **/
bool PWDefault::DssrStandard(){
    bool isElementary = true;

    Bitset repeated_visits(problem->getNumNodes());
    auto tour = getBestSolution()->getTour();
    tour.sort();
    int current_node = -1;

    for(auto t: tour) {
        if(t == current_node) {
            isElementary = false;
            repeated_visits.set(current_node);
        }
        else current_node = t;
    }

    //update unreachables
    int u_count;
    if(!isElementary) {
        for(auto & u: unreachable_active) {
            u |= repeated_visits;
            u_count = u.count();
            if(u_count > unreachable_max_count)
                unreachable_max_count = u_count;
        }
    }

    return isElementary;
}

bool PWDefault::DssrRestricted(){
    bool isElementary = true;

    auto tour = getBestSolution()->getTour();
    int scanned_node;
    std::list<int> scanned_nodes;
    Bitset unique_visits(problem->getNumNodes()), cycle_visits(problem->getNumNodes());

    int current_node;
    for(auto it = tour.begin(); it != tour.end(); it++) {
        current_node = *it;
        if(unique_visits.get(current_node)){
            for(auto it_scan = scanned_nodes.rbegin(); it_scan != scanned_nodes.rend(); it_scan++) {
                scanned_node = *it_scan;
                cycle_visits.set(scanned_node);
                if(current_node == scanned_node) {
                    isElementary = false;

                    for(int i = 0; i < cycle_visits.size(); i++)
                        if(cycle_visits.get(i))
                            unreachable_active[i].set(current_node);

                    break;
                }
            }
            cycle_visits.reset();
        }
        else unique_visits.set(current_node);
        scanned_nodes.push_back(current_node);
    }

    int u_count;
    for(auto t: tour){
        u_count = unreachable_active[t].count();
        if(u_count > unreachable_max_count)
            unreachable_max_count = u_count;
    }

    return isElementary;
}

void PWDefault::buildNG(){
    if(ng_size <= 1) return;

    ng_compliant = true;
    auto objective = problem->getObj();

    unreachable_ng.resize(problem->getNumNodes(), Bitset(problem->getNumNodes()));
    std::set<std::pair<int, int>> neighbors;

    //Arc-cost based ordering (should take into account extension instead)
    int neighborhood_max_size = ng_size - 1; //To account for node i in the set
    for(int i = 0; i < problem->getNumNodes(); i++) {
        for(int j = 0; j < problem->getNumNodes(); j++)
            if(problem->areNeighbors(i, j, true)) {
                if(neighbors.size() < neighborhood_max_size)
                    neighbors.insert(std::make_pair(objective->extend(0,i,j,true), j));
                else if(objective->extend(0,i,j,true) < neighbors.rbegin()->first) {
                        neighbors.erase(std::prev(neighbors.end()));
                        neighbors.insert(std::make_pair(objective->extend(0,i,j,true), j));
                }
            }

        for(auto & n: neighbors)
            unreachable_ng[i].set(n.second);
        unreachable_ng[i].set(i);
        neighbors.clear();
    }

    if(ng == NG_STANDARD) {
        unreachable_active = unreachable_ng;
        unreachable_max_count = unreachable_active[0].count();
    }
}

bool PWDefault::NgRestricted() {
    bool isNG = true;
    Bitset visited_memory(problem->getNumNodes());
    auto tour = getBestSolution()->getTour();
    std::list<int> scanned_nodes;
    int scanned_node;

    for(auto t: tour) {
        //If visited has memory of seeing a node that is part of the ng-route: cycle on t is forbidden
        if(visited_memory.get(t)){
            isNG = false;
            //Bit to 1 in position t for each node of the closest cycle
            for(auto it_scan = scanned_nodes.rbegin(); it_scan != scanned_nodes.rend(); it_scan++) {
                scanned_node = *it_scan;
                unreachable_active[scanned_node].set(t);
                if(scanned_node == t) break;
            }
        }
        scanned_nodes.push_back(t);
        visited_memory &= unreachable_ng[t];
        visited_memory.set(t);
    }

    int u_count;
    for(auto t: tour){
        u_count = unreachable_active[t].count();
        if(u_count > unreachable_max_count)
            unreachable_max_count = u_count;
    }

    return isNG;
}

std::string PWDefault::getRelaxationName() {
    std::string ng_name, dssr_name, relaxation_name;

    switch(ng){
        case NG_STANDARD: ng_name = "NG standard"; break;
        case NG_RESTRICTED: ng_name = "NG restricted"; break;
    }

    switch(dssr){
        case DSSR_STANDARD: dssr_name = "DSSR standard"; break;
        case DSSR_RESTRICTED: dssr_name = "DSSR restricted"; break;
    }

    relaxation_name = ng_name;
    if(!relaxation_name.empty() and !dssr_name.empty())
        relaxation_name += " + ";
    relaxation_name += dssr_name;

    return relaxation_name.empty() ? "no relaxation" : relaxation_name;
}


/** Debug **/
void PWDefault::preGenLabels(std::list<int> tour, bool direction){
    if(not direction)
        tour.reverse();
    tour.pop_front();
    int next_node;

    //Starting label from origin or destination
    LabelAdv* current_label = label_manager->getLabel(direction, 0);
    LabelAdv new_label = LabelAdv();

    while(not tour.empty()) {
        next_node = tour.front();
        tour.pop_front();
        label_manager->extendLabel(current_label, &new_label, next_node);
        label_manager->updateUnreachables(&new_label);
        current_label = label_manager->insert(&new_label);
    }
}

/** Data Collection management **/
void PWDefault::initDataCollection() {
    timeout = false;

    collector.setCollectionName(name);

    if(not Parameters::isCollecting())
        return;

    label_manager->setName(name + "_LM");

    collector.collect("algo_name", name);
    collector.collect("algo_type", algo_type);
    if(bidirectional)
        collector.collect("bidirectional", true);

    //Initialize data collection of new values
    collector.initTime("t_labeling");
    collector.initTime("t_ext_fw");
    collector.initTime("t_ext_bw");
    if(Parameters::getCollectionLevel() >= 1){
        collector.initTime("t_ins_fw");
        collector.initTime("t_ins_bw");
    }
    collector.initTime("t_join");

    collector.init("it_ext_fw", 0);
    collector.init("it_ext_bw", 0);
    collector.init("ins_attempts_fw", 0);
    collector.init("ins_attempts_bw", 0);
    collector.init("relaxation", getRelaxationName());
    collector.init("neighbourhood_max_size", unreachable_max_count);

    collector.init("NGroute_compliant", -1);

    collector.setHeader();
}

void PWDefault::collectData(){
    collector.collect("executionID", executionID);

    //Collect Data at the end of iteration
    if(not Parameters::isCollecting())
        return;

    collector.collect("iterations", iterations);
    collector.collect("it_ext_fw", it_ext_fw);
    collector.collect("it_ext_bw", it_ext_bw);
    collector.collect("ins_attempts_fw", ins_attempts_fw);
    collector.collect("ins_attempts_bw", ins_attempts_bw);
    collector.collect("lb", lower_bound);
    collector.collect("ub", incumbent);
    collector.collect("algo_status", algo_status);
    if(ng != NG_OFF)
        collector.collect("NGroute_compliant", ng_compliant);
    collector.collect("timeout", timeout);
    collector.collect("neighbourhood_max_size", previous_unreachable_max_count);

    collector.saveRecord();
}

void PWDefault::writeData(){
    collector.writeData();
    collector_sol.writeData();
    label_manager->writeData();
}