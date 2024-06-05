#include "solver.h"
#include <filesystem>

/** Solver Management **/

/**
 * Solver Constructor. Builds the solver.
 */
Solver::Solver() {
    setStatus(SOLVER_START);
    solver_version = "0.1";
    optimization_round = 0;
    problem = nullptr;
    main_algorithm = nullptr;
    readConfiguration();
    setupOutput();
    printWelcome();
}

/**
 * Solver Deconstructor. Destroys the solver.
 */
Solver::~Solver() {
    delete main_algorithm;
    delete problem;

    for(auto &a: ensemble_algorithms)
        delete a;
}

// General solver methods

/**
 * Prints welcome message and version to console.
 */
void Solver::printWelcome() {
    if(Parameters::getVerbosity() < 0)
        return;

    std::cout << "PathWyse ver. " << solver_version << std::endl;
    std::cout << "--------------------" << std::endl;
}

/**
 * Reads and setups settings from configuration file.
 * 
 * @param file_path - Configuration file path.
 */
void Solver::readConfiguration(std::string file_path){
    Parameters::readParameters(file_path);
    default_instance = Parameters::getInstancePath();
    main_algorithm_name = Parameters::getMainAlgorithmName();
    ensemble_algorithms_names = Parameters::getEnsembleNames();
    use_ensemble = Parameters::isEnsembleUsed();
}

/**
 * Setups output folders for data collection.
 */
void Solver::setupOutput() {
    //Setup solution collector
    collector = DataCollector("Solutions");
    std::cout<<std::fixed;

    if(not Parameters::isOutputStored())
        return;

    //Setup output folders
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%F-%T");
    if (oss.str().length() <= 1) {
        oss.str("");
        oss.clear();
        oss << std::put_time(std::localtime(&now), "%Y-%m-%d_%H-%M-%S");
    }
    Parameters::setTimestamp(oss.str());

    //Creates folders for the experiments
    Parameters::setupCollectionPath();
    std::string data_collection_path = Parameters::getCollectionPath();

    if(not data_collection_path.empty())
        std::filesystem::create_directories(data_collection_path);

    //Store set file for experiment
    if(std::filesystem::exists("pathwyse.set"))
        std::filesystem::copy("pathwyse.set", data_collection_path + "pathwyse.set");
}

/**
 * Sets console verbosity.
 *
 * @param verbosity - Verbosity level.
 */
void Solver::setConsoleVerbosity(int verbosity) {
    Parameters::setVerbosity(verbosity);
}

// Problem management

/**
 * Reads problem instance from file.
 * 
 * @param file_name - Name of the problem file.
 */
void Solver::readProblem(std::string file_name) {
    problem = new Problem();

    if(file_name.empty())
        file_name = default_instance;

    problem->readProblem(file_name);
    scaleData();
    setStatus(SOLVER_READY);
}

/**
 * Sets already definied custom problem
 * 
 * @param problem - Problem object (pointer).
 */
void Solver::setCustomProblem(Problem& problem) {
    this->problem = &problem;
    scaleData();
    setStatus(SOLVER_READY);
}

/**
 * Gets the number of nodes in the network
 * 
 * @return int - Number of nodes in the network.
 */
int Solver::getNumberOfNodes() {
    return problem ? problem->getNumNodes() : 0;
}

/**
 * Sets initial cost of the problem.
 * 
 * @param cost - Cost value.
 */
void Solver::setInitCost(int cost){
    if(problem)
        problem->getObj()->setInitValue(cost);
}

/**
 * Sets cost for a node.
 * 
 * @param id - Id of the node.
 * @param cost - Node cost.
 */
void Solver::setNodeCost(int id, int cost){
    if(problem)
        problem->getObj()->setNodeCost(id, cost);
}

/**
 * Sets costs for all nodes.
 * 
 * @param costs - Vector of costs to assing to problem nodes.
 */
void Solver::setNodeCost(std::vector<int> costs) {
    if(problem)
        problem->getObj()->setNodeCosts(costs);
}

/**
 * Scales data function (objective, resource or everything). Makes use of scaling parameter.
 */
void Solver::scaleData() {
    if(not Parameters::isScalingOverridden())
        return;

    std::string target = Parameters::getScalingTarget();
    if(target == "objective")
        problem->scaleObjective(Parameters::getScaling());
    else if(target.substr(0, 3) == "res" and target.length() >= 4){
        int index = target[3] - '0';
        if(index < problem->getNumRes())
            problem->scaleResource(index, Parameters::getScaling());
    }
    else
        problem->scaleAllData(Parameters::getScaling());

    if(Parameters::getVerbosity() >= 1) {
        std::cout<< "Performed scaling on target: " << target << std::endl;
        std::cout<< "Scaling: " << Parameters::getScaling() << std::endl;
    }
}

/**
 * Reverts scaled data back to the original values.
 */
void Solver::revertScaleData() {
    if(not Parameters::isScalingOverridden())
        return;

    float inv_scaling = 1/Parameters::getScaling();

    std::string target = Parameters::getScalingTarget();
    if(target == "objective")
        problem->scaleObjective(inv_scaling);
    else
        problem->scaleAllData(inv_scaling);

    if(Parameters::getVerbosity() >= 1)
        std::cout<< "Reverted scaling on target: " << target << std::endl;
}

// Algorithms management

/**
 * Setups algorithms.
 */
void Solver::setupAlgorithms() {
    if (problem) {
        setMainAlgorithm(main_algorithm_name);
        setEnsemble(ensemble_algorithms_names);
    }
    else std::cout<<"Warning: no problem defined"<<std::endl;
}

/**
 * Creates algorithm object.
 * 
 * @param name - Algorithm name.
 * @return Algorithm* - New algorithm pointer.
 */
Algorithm* Solver::createAlgorithm(std::string name){
    if(name == "PWAcyclic")
        return new PWAcyclic(name, problem);
    else
        return new PWDefault(name, problem);
}

// Algorithms: setters (name)

/**
 * Sets a new main algorithm. Deletes any previous algorithm object.
 * 
 * @param name - Algorithm name.
 */
void Solver::setMainAlgorithm(std::string name) {
    delete main_algorithm;
    main_algorithm = createAlgorithm(name);
}

/**
 * Sets new ensemble of algorithms. Clears the current ensemble.
 * 
 * @param names - Names of algorithms to add to ensemble.
 */
void Solver::setEnsemble(std::vector<std::string> names) {
    //Reset Data Structure
    clearEnsemble();

    for(int a = 0; a < names.size(); a++)
        ensemble_algorithms.push_back(createAlgorithm(names[a]));
}

/**
 * Adds one algorithm object to the ensemble.
 * 
 * @param name - Name of the algorithm to add to the ensemble.
 */
void Solver::addEnsembleAlgorithm(std::string name) {
    ensemble_algorithms.emplace_back(createAlgorithm(name));
}

/**
 * Changes one algorithm in the ensemble.
 * 
 * @param id - Position of the algorithm in the ensemble to change.
 * @param name - Name of the new algorithm.
 */
void Solver::changeEnsembleAlgorithm(int id, std::string name){
    if(id < ensemble_algorithms.size()){
        delete ensemble_algorithms[id];
        ensemble_algorithms[id] = createAlgorithm(name);
    }
}

/**
 * Solves Problem with either main or ensemble algorithms.
 */
void Solver::solve() {
    optimization_round++;

    setStatus(SOLVER_BUSY);

    if(Parameters::getVerbosity() >= 0) {
        std::cout<<"Solving problem..."<<std::endl;
        if(Parameters::getVerbosity() >= 1)
            std::cout<<"Algorithm mode: " << (isEnsembleUsed() ? "ensemble" : "main algorithm") << std::endl;
    }

    isEnsembleUsed() ? solveEnsemble() : solveAlgorithm(MAIN_ALGORITHM);

    if(Parameters::getVerbosity() >= 0)
        std::cout<<"--------------------"<<std::endl;

    printStatus();
    setStatus(SOLVER_READY);
}

/**
 * Solves Problem with ensembled algorithms.
 */
void Solver::solveEnsemble() {
    for(int id = 0; id < ensemble_algorithms.size(); id++)
        solveAlgorithm(id);
}

/**
 * Solves the problem with a specified algorithm. Private method used by solve() and solveEnsemble().
 * 
 * @param id - Identifier of the algorithm to solve. If id is equal to -1 use the main algorithm, otherwise use the id-th algorithm of the ensemble.
 */
void Solver::solveAlgorithm(int id) {
    Algorithm* algorithm;

    if(id == MAIN_ALGORITHM)
        algorithm = main_algorithm;
    else if (id < ensemble_algorithms.size())
        algorithm = ensemble_algorithms[id];
    else
        return;

    algorithm->setExecutionID(optimization_round);
    algorithm->solve();
    std::vector<Path> algorithm_solutions = algorithm->getSolutions();

    if(not algorithm_solutions.empty())
        solutions.insert(solutions.end(), algorithm_solutions.begin(), algorithm_solutions.end());

    if(Parameters::getVerbosity() >= 0)
        std::cout<< algorithm->getName() << " global time: " << algorithm->getGlobalTime() << std::endl;

}

// Reset algorithms

/**
 * Resets the main algorithm.
 * 
 * @param reset_level - Reset level.
 */
void Solver::resetMainAlgorithm(int reset_level) {
    if(main_algorithm)
        main_algorithm->resetAlgorithm(reset_level);}

/**
 * Resets algorithm in the ensemble.
 * 
 * @param id - Ensemble algorithm position to reset.
 * @param reset_level - Reset level.
 */
void Solver::resetEnsembleAlgorithm(int id, int reset_level){
    if(id <ensemble_algorithms.size() and ensemble_algorithms[id])
        ensemble_algorithms[id]->resetAlgorithm(reset_level);
}

/**
 * Resets all algorithms in the ensemble.
 * 
 * @param reset_level - Reset level.
 */
void Solver::resetEnsemble(int reset_level){
    for(auto & a: ensemble_algorithms)
        a->resetAlgorithm(reset_level);
}

// Clear algorithms

/**
 * Clears main algorithm.
 */
void Solver::clearMainAlgorithm() {
    delete main_algorithm;
    main_algorithm = nullptr;
}

/**
 * Clears algorithms in the ensemble.
 */
void Solver::clearEnsemble() {
    for(auto* a: ensemble_algorithms)
        delete(a);
    ensemble_algorithms.clear();
}

/** Solution Management **/

/**
 * Orders solutions by a given criteria. By default they are ordered by objective function value.
 * 
 * @param criteria - Ranking criteria.
 */
void Solver::rankSolutions(std::string criteria){
    if(criteria == RANK_OBJECTIVE)
        std::sort(solutions.begin(), solutions.end(), less_than_objective());
}

/**
 * Gets solution.
 * 
 * @param id - Id of the solution to get.
 * @return Path* - Solution Path object (pointer).
 */
Path* Solver::getSolution(int id) {
    return id < solutions.size() ? & solutions[id] : nullptr;
}

/**
 * Gets solution status.
 * 
 * @param id - Id of the solution from which retrieve the status.
 * @return int - Solution status.
 */
int Solver::getSolutionStatus(int id) {
    return id < solutions.size() ? solutions[id].getStatus() : PATH_UNKNOWN;
}

/**
 * Gets solution objective.
 * 
 * @param id - Id of the solution from which retrieve the objective.
 * @return int - Solution objective.
 */
int Solver::getSolutionObjective(int id){
    return id < solutions.size() ? solutions[id].getObjective() : UNKNOWN;
}

/**
 * Gets solution arc cost
 * 
 * @param id - Id of the solution from which retrieve Arc Cost.
 * @return int - Solution arc cost.
 */
int Solver::getSolutionArcCost(int id) {
    return id < solutions.size() ? solutions[id].getArcCost() : UNKNOWN;
}

/**
 * Gets solution node cost.
 * 
 * @param id - Id of the solution from which retrieve node cost.
 * @return int - Solution node cost.
 */
int Solver::getSolutionNodeCost(int id) {
    return id < solutions.size() ? solutions[id].getNodeCost() : UNKNOWN;
}

/**
 * Gets tour of a solution.
 * 
 * @param id - Position of the solution from which the tour is retrieved.
 * @return std::vector<int> - Solution tour.
 */
std::vector<int> Solver::getSolutionTour(int id){
    if(id >= solutions.size())
        return {};

    auto tour_list = getSolution(id)->getTour();
    std::vector<int> tour_vec(tour_list.begin(), tour_list.end());
    return tour_vec;
}

/**
 * Gets tour of a solution, as a string.
 * 
 * @param id - Position of the solution from which retrieve solution tour as string.
 * @return std::string - Solution tour as string.
 */
std::string Solver::getSolutionTourAsString(int id) {
    return id < solutions.size() ? solutions[id].getTourAsString() : "";
}

/**
 * Returns a pool of the best solutions found.
 * 
 * @param pool_size - Number of top solutions to return.
 * @return std::vector<Path> - Vector of solutions.
 */
std::vector<Path> Solver::getBestSolutions(int pool_size){
    rankSolutions(RANK_OBJECTIVE);

    if(pool_size >= solutions.size())
        return solutions;

    std::vector<Path> top_solutions (pool_size);
    std::copy_n(solutions.begin(), pool_size, top_solutions.begin());
    return top_solutions;
}

/**
 * Returns the best solution found.
 * 
 * @return Path - Best solution found.
 */
Path Solver::getBestSolution(){
    if(solutions.empty())
        return Path();

    rankSolutions(RANK_OBJECTIVE);
    return solutions[0];
}

/** Output management **/

/**
 * Prints problem and algorithm status.
 */
void Solver::printStatus() {
    if(Parameters::getVerbosity() < 0)
        return;

    problem->printStatus();

    if(Parameters::getVerbosity() >= 1)
        isEnsembleUsed() ? printEnsembleStatus() : main_algorithm->printStatus();
    std::cout<<"--------------------"<<std::endl;
}

/**
 * Prints best solution found.
 */
void Solver::printBestSolution() {
    if(Parameters::getVerbosity() > 0)
        std::cout<<"Best solution found:"<<std::endl;

    if(solutions.size() > 0)
        getBestSolution().printPath();
    else
        std::cout<< "No solution found" <<std::endl;
}

/**
 * Prints ensemble status.
 */
void Solver::printEnsembleStatus(){
    for(auto & algo: ensemble_algorithms)
        algo->printStatus();
}

/**
 * Prints node costs.
 */
void Solver::printNodeCosts() {
    for(int i = 0; i < problem->getNumNodes(); i++)
        std::cout<<i<<") "<<problem->getObj()->getNodeCost(i) << " ";
    std::cout<<std::endl;
}