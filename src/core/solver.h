#ifndef PWSOLVER_H
#define PWSOLVER_H

#include "data/problem.h"
#include "data/path.h"
#include "algorithms/dynamic_programming/PW_default/PW_default.h"
#include "algorithms/dynamic_programming/PW_acyclic/PW_acyclic.h"

class Solver{

public:

    /** Solver management **/
    Solver();
    ~Solver();

    //General solver methods
    void printWelcome();
    bool getStatus(){return solver_status;}
    void setStatus(int status) {solver_status = status;}

    //Setup
    void readConfiguration(std::string file_path = "");
    void setupOutput();

    //Additional interfaces
    void setConsoleVerbosity(int verbosity);

    /** Problem management **/
    void readProblem(std::string file_name = "");                                           //Use default problem/reader
    void setCustomProblem(Problem& problem);                                                //Set a custom problem
    Problem* getProblem() {return problem;}                                                 //Get problem pointer
    int getNumberOfNodes();
    void setInitCost(int cost);
    void setNodeCost(int id, int cost);
    void setNodeCost(std::vector<int> costs);
    void scaleData();
    void revertScaleData();

    /** Algorithms management **/
    void setupAlgorithms();
    Algorithm* createAlgorithm(std::string name);

    //Algorithms: setters (name)
    void setMainAlgorithm(std::string name);
    void setEnsemble(std::vector<std::string> names);
    void addEnsembleAlgorithm(std::string name);
    void changeEnsembleAlgorithm(int id, std::string name);

    //Algorithm: getters (pointer)
    Algorithm* getMainAlgorithm(){return main_algorithm;}
    Algorithm* getEnsembleAlgorithm(int id);

    //Solve Problem with selected Algorithms
    void solve();
    void solveEnsemble();
    void solveAlgorithm(int algorithm_code);

    //Reset algorithms
    void resetMainAlgorithm(int reset_level);
    void resetEnsembleAlgorithm(int id, int reset_level);
    void resetEnsemble(int reset_level);

    //Clear algorithms
    void clearMainAlgorithm();
    void clearEnsemble();

    /** Solution management **/
    int getNumberOfSolutions() {return solutions.size();}
    void rankSolutions(std::string criteria = "objective");
    Path* getSolution(int id);
    int getSolutionStatus(int id);
    int getSolutionObjective(int id);
    int getSolutionArcCost(int id);
    int getSolutionNodeCost(int id);
    std::vector<int> getSolutionTour(int id);
    std::string getSolutionTourAsString(int id);
    std::vector<Path> getAllSolutions(){return solutions;}
    std::vector<Path> getBestSolutions(int pool_size = 1);
    Path getBestSolution();
    void clearSolutions(){solutions.clear();}

    /** Output management **/
    void printStatus();
    void printBestSolution();
    void printEnsembleStatus();
    void printNodeCosts();

    /** Parameters interfaces **/
    bool isEnsembleUsed()  {return use_ensemble;}
    void useEnsemble(bool use_ensemble) {this->use_ensemble = use_ensemble;}

private:

    //Solver
    std::string solver_version;                         //PathWyse version number
    int solver_status;                                  //Solver status
    int optimization_round;                             //Optimization round

    //Problem
    Problem* problem;                                   //Problem
    std::string default_instance;                       //Instance name

    //Main Algorithm
    Algorithm* main_algorithm;                          //Main algorithm
    std::string main_algorithm_name;                    //Main algorithm name

    //Ensemble Algorithms
    bool use_ensemble;
    std::vector<Algorithm*> ensemble_algorithms;        //Ensemble algorithms
    std::vector<std::string> ensemble_algorithms_names; //Ensemble algorithms names

    //Solutions
    std::vector<Path> solutions;

    //Data collection
    DataCollector collector;

};
#endif
