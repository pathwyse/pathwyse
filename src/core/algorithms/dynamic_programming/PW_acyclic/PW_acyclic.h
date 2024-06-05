#ifndef PW_ACYCLIC_H
#define PW_ACYCLIC_H

#include "algorithms/algorithm.h"
#include "algorithms/preprocessing/preprocessing.h"
#include "LM_acyclic.h"

class PWAcyclic: public Algorithm {

public:

    /** Algorithm management **/
    //Constructors and destructors
    PWAcyclic(std::string name, Problem* problem);
    ~PWAcyclic();

    //Initialization
    void initAlgorithm();

    //Preprocessing and search algorithm
    bool preprocessing();
    void solve() override;
    void extend(Label* candidate);

    //Reset
    void resetAlgorithm(int reset_level) override;

    //Build Path
    void managePaths();

    /** Data collection management **/
    void initDataCollection();
    void collectData();

private:
    Preprocessing* preprocess;
    LMacyclic* label_manager;

    //Data collection
    int it_dominated;
    int it_ext_fw, it_ext_bw;
    int ins_attempts_fw, ins_attempts_bw;
};

#endif



