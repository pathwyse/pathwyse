#ifndef SPPRCLIB_DP_BIDIRECTIONAL_H
#define SPPRCLIB_DP_BIDIRECTIONAL_H

#include "algorithms/algorithm.h"
#include "LM_default.h"
#include "algorithms/labels/label_advanced.h"

class PWDefault: public Algorithm {

public:

    /** Algorithm management **/
    //Constructors and destructors
    PWDefault(std::string name, Problem* problem);
    ~PWDefault();

    //Init and reset
    void initAlgorithm();
    void readConfiguration();

    void resetIteration();
    void resetAlgorithm(int reset_level);

    //Solve
    void solve() override;
    void labeling(bool forward = true, bool backward = true);
    void extend(LabelAdv* candidate);
    bool checkTermination();

    //Path building
    void managePaths();

    /** Relaxation management **/
    void buildNG();            //build NG sets
    bool DssrStandard();                //DSSR
    bool DssrRestricted();     //iteratively DSSR forbid only repeating customers in cycles
    bool NgRestricted();       //iteratively forbid only cycles that are not part of an NG route
    std::string getRelaxationName();

    /** Debug **/
    void preGenLabels(std::list<int> tour, bool direction);

    /** Data Collection management **/
    void initDataCollection();
    void collectData();
    void writeData();

protected:

    //Label Manager
    LMDefault* label_manager;

    //Parameters (Configuration)
    float timelimit;            //timelimit (s)
    bool use_visited;           //check
    int dssr;                   //DSSR mode
    int ng;                     //NG mode
    int ng_size;
    bool earlyjoin;
    unsigned long long int earlyjoin_step;

    //Unreachable nodes
    std::vector<Bitset> unreachable_active;
    std::vector<Bitset> unreachable_ng;

    //Parameters (Data collection)
    int unreachable_max_count, previous_unreachable_max_count;
    bool timeout, ng_compliant;
    int it_ext_fw, it_ext_bw, ins_attempts_fw, ins_attempts_bw;
};

#endif //SPPRCLIB_DP_BIDIRECTIONAL_H
