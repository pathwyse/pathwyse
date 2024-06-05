#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "algorithms/algorithm.h"
#include "algorithms/labels/label.h"
#include <queue>
#include "utils/bitset.h"

class Dijkstra: public Algorithm {

public:
    /** Algorithm management **/
    //Constructors and destructors
    explicit Dijkstra(std::string name, Problem* problem);
    ~Dijkstra() = default;

    void initAlgorithm(bool direction, int res_id, double bounding);

    //Solution process
    void solve() override;
    void extendLabel(Label* current_label, Label & new_label, int next_node);
    bool isLabelValid(Label* l, int cost);
    bool isCriticalLabelValid(Label* l, int cost);
    void checkFeasibility();
    void checkOptimality();

    //Join
    void joinCompletion(Label* current_label);
    bool joinLabels(Label* l1, Label* l2);

    void resetAlgorithm(int reset_level) override;

    /** Query **/
    Bitset & getVisited(){return visited;}
    //Returns the minimum distance for a node (from the source)
    int getDistance(int node){return distances[node];}
    //Returns the distance of a label (from the source)
    int getDistance(Label * l){return res_id == RES_COST ? l->getObjective() : l->getSnapshot(res_id);}

    /** Solution management **/
    void managePaths();
    std::list<Label> buildTour(std::list<int> tour, bool direction);
    bool foundOptimal(){return found_optimal;}

private:

    //Optimization Run information
    int round;
    int origin;
    int destination;
    bool direction;
    double bounding;                //Resource split
    int res_id;
    int found_optimal;

    //Resource and completion labels pointers
    Resource* obj;
    Resource* res;
    BoundLabels* bound_labels;

    //Solution information
    std::vector<Label>* labels;

    Bitset visited;
    std::vector<int> distances;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

    std::tuple<int, Label*, Label*> solution_data;
};

#endif

