#ifndef LABELMANAGER_H
#define LABELMANAGER_H

#include <set>
#include "algorithms/labels/label_advanced.h"
#include "data/problem.h"

//Queue based Label Manager for PW_default.
//Handles data structures and returns candidates for the extension and join steps

class LMDefault{

public:

    /** LM management **/
    //Constructors and destructors
    LMDefault(Problem* problem);
    ~LMDefault() = default;

    //Initialization
    void initLM();
    void readConfiguration();

    void resetLM();

    //Support methods
    void setName(std::string name) {this->name = name; collector.setCollectionName(name);}
    void setProblem(Problem* problem) {this->problem = problem;}
    void setUseVisited(bool use_visited) {this->use_visited = use_visited;}
    void setCompareUnreachables(bool compare_unreachables) {this->compare_unreachables = compare_unreachables;}

    //Bidirectional budget management
    void update_split();
    void setSplit(double split){this->split_ratio = split;}
    double getSplit() {return split_ratio;}

    //Incumbent
    int getIncumbent(){return incumbent;}
    void setIncumbent(int incumbent){this->incumbent = incumbent;}

    //Queries
    int getForwardSize() {return forward_labels.size();}
    int getBackwardSize() {return backward_labels.size();}
    int totalLabels(){return (forward_labels.size() + backward_labels.size());}
    int candidateSize(bool direction, int id) {return direction ? forward_candidates[id].size() : backward_candidates[id].size();}
    int closedSize(bool direction, int id) {return direction ? forward_closed[id].size() : backward_closed[id].size();}
    LabelAdv* getLabel(bool direction, int index) {return direction? & forward_labels[index] : & backward_labels[index];}

    /** Candidate management **/
    bool candidatesAvailable(bool forward = true, bool backward = true);
    LabelAdv* getCandidate(bool forward = true, bool backward = true);
    LabelAdv* getCandidateRR(bool forward, bool backward);
    LabelAdv* getCandidateNode(bool forward, bool backward);

    /** Label management**/
    //Label extension
    bool isNodeReachable(LabelAdv *label, int next_node);
    bool isExtensionFeasible(LabelAdv *label, int next_node);
    bool isCriticalExtensionFeasible(LabelAdv *label, int next_node);
    void extendLabel(LabelAdv *current_label, LabelAdv *new_label, int next_node);
    void updateUnreachables(LabelAdv *label);

    //Label insertion
    LabelAdv* insert(LabelAdv *new_label);
    bool dominates (LabelAdv *l1, LabelAdv *l2) const;

    /** Join **/
    void join();
    void closeLabels();
    void restoreClosedLabels();
    void naiveJoin();
    void classicJoin();
    void orderedJoin();
    bool isJoinFeasible(LabelAdv* forward, LabelAdv* backward);                 //Checks if two labels can be joined
    bool joinFound() {return not joinable_labels.empty();}                      //Checks if a join was found

    std::tuple<int, LabelAdv*, LabelAdv*> getBestJoin() {return *joinable_labels.begin();}       //returns best join
    std::multiset<std::tuple<int, LabelAdv*, LabelAdv*>> getAllJoin() {return joinable_labels;}

    /** Solution management **/
    std::tuple<int, LabelAdv*, LabelAdv*> getSolutionLabels();
    void setODLabel();
    LabelAdv* getODLabel(){return (od_label.second >= 0) ? getLabel(od_label.first, od_label.second) : nullptr;}

    /** Heuristic management **/
    void setQueueLimit(int queue_limit){if(queue_limit > 0) this->queue_limit = queue_limit;}

    /** Debug **/
    std::list<LabelAdv> buildTour(std::list<int> tour, bool direction);
    void findLabels(std::list<LabelAdv> * tourLabels);
    void printStepConsumption(std::list<LabelAdv> *tourFW, std::list<LabelAdv> *tourBW);
    void printClosed(int id, bool direction = true);
    void printCandidates(int id, bool direction = true);

    /** Data collection management **/
    void initDataCollection();
    void setExecutionID(int id){executionID = id;}
    void setIteration(int iterations){this->iterations = iterations;}
    float getMeanLabels(bool direction);
    int getMaxLabels(bool direction);
    float getVarLabels(bool direction, float mean);
    float getJoinComparisons(){return joinComparisons;}
    void collectData();
    void writeData() {collector.writeData();}

protected:

    std::string name;
    std::string lm_type;

    //Problem
    Problem* problem;

    //Parameters
    bool bidirectional;
    bool autoconfiguration;

    int candidate_type;
    int join_type;

    int queue_limit;
    int reserve_size;
    int turn_forward, turn_backward;
    double split_ratio;
    int incumbent;
    bool use_visited, compare_unreachables;

    int ndominated_fw, ndominated_bw;
    int nclosed_fw, nclosed_bw;

    //direction, index in the storage
    std::pair<bool, int> od_label;

    /** Label pools, masks, and pointers **/
    std::vector<LabelAdv> forward_labels, backward_labels;  //Pool of all available forward (resp. backward) labels

    //For each node, store an ordered list of labels (pointer)
    //Open labels
    std::vector<std::list<std::pair<int, int>>> forward_candidates, backward_candidates;            //Open labels
    LabelAdv *best_fw, *best_bw;                                                                    //Best overall labels
    std::vector<LabelAdv*> forward_best, backward_best;                                             //Best overall LabelAdv for each node
    std::multiset<std::pair<int, int>> forward_top_candidates, backward_top_candidates;             //Best available candidates, ordered

    //Closed labels
    std::vector<std::vector<std::pair<int, int>>> forward_closed, backward_closed;                      //Closed labels
    std::vector<std::vector<std::pair<int, int>>> forward_closed_backup, backward_closed_backup;        //Backup of closed labels

    std::multiset<std::tuple<int, LabelAdv*, LabelAdv*>> joinable_labels;                 //Joinable label pairs

    //Data collection
    int executionID;
    int iterations;
    unsigned long long joinComparisons;
    DataCollector collector;
};

#endif

