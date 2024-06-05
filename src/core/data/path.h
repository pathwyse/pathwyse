#ifndef PATH_H
#define PATH_H

#include <list>
#include "algorithms/labels/label.h"

class Path {

public:

    /** Path management **/
    //Constructors and destructors
    Path() {initPath();}
    Path(const Path &obj);
    ~Path() = default;

    void initPath();
    void reset() {initPath();}

    /** Tour management **/
    std::list<int> getTour(){return tour;}
    std::string getTourAsString();
    void setTour(std::list<int> tour);
    int getTourLength(){return tour.size();}

    //Elementarity management
    bool isElementary() {return elementary;}
    void setElementary(bool elementary) {this->elementary = elementary;}
    bool checkElementarity();

    /** Status and solution management **/
    int getStatus(){return solution_status;}
    void setStatus(int status){solution_status = status;}

    //Objective and resource information
    int getObjective(){return objective;}
    void setObjective(int objective){this->objective = objective;}
    int getArcCost() {return arc_cost;}
    void setArcCost(int cost) {this->arc_cost = cost;}
    int getNodeCost() {return node_cost;}
    void setNodeCost(int cost) {this->node_cost = cost;}
    int getConsumption(int id){return consumption[id];}
    std::vector<int> getConsumptions(){return consumption;}
    void setConsumption(std::vector<int> consumption){this->consumption = consumption;}

    //Optimality guarantee
    bool isOptimal() {return solution_status == PATH_OPTIMAL;}
    bool isFeasible(){return solution_status == PATH_FEASIBLE;}

    /** Print path information **/
    void printPath();
    void printStatus();

    /** Labels management **/
    void setLabels(std::list<Label> labels){this->labels = labels;}
    std::list<Label> & getLabels(){return labels;}


private:
    int solution_status;                 //Solution status

    std::list<int> tour;                 //Tour elements
    bool elementary;

    int objective;                      //Objective value
    int arc_cost, node_cost;
    std::vector<int> consumption;       //Resources consumption

    std::list<Label> labels;            //Path labels
};

/** Path Comparison **/
//Objective based
struct less_than_objective
{
    bool operator() (Path& p1, Path& p2){
        return (p1.getObjective() < p2.getObjective());
    }
};

#endif