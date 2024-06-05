#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "algorithms/algorithm.h"
#include "algorithms/preprocessing/dijkstra.h"

class Preprocessing: public Algorithm {

public:

    /** Algorithm management **/
    //Constructors and destructors
    explicit Preprocessing(std::string name, Problem* problem);
    ~Preprocessing() {delete dijkstra;}

    //Solution process
    void solve() override;
    bool solveRound(bool direction, int res_id, double bounding = 1);

    void resetAlgorithm(int reset_level) override;

    //Compute split
    double getSplit();

    /** Pruning management **/
    void pruning();
    void pruneCritical();

    /** Data collection management **/
    void initDataCollection();
    void collectData();

protected:
    int round;

    bool preprocessingCritical;
    Bitset criticalActive;

    Dijkstra* dijkstra;
};

#endif
