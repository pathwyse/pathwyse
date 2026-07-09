#ifndef GRAPH_H
#define GRAPH_H



#include <vector>
#include <map>
#include <cmath>
#include "utils/constants.h"
#include "utils/bitset.h"

class Graph {

public:

    /** Graph management **/
    //Constructors and destructors
    Graph() {}
    ~Graph() = default;

    //Data structure initialization
    void initGraph(bool compress_data, int n_nodes);

    /**  Coordinates and distances management **/
    void initCoord(){x.resize(n_nodes); y.resize(n_nodes);}
    void setxs(std::vector<int> x) {this->x = x;}
    void setys(std::vector<int> y) {this->y = y;}
    void setxy(int i, int xval, int yval){x[i] = xval; y[i] = yval;}
    std::pair<int, int> getCoordinates(int node) {return std::make_pair(x[node], y[node]);}
    int getCoordDistance(int i, int j, int distance_algo);
    double getEuclideanDistance(double x1, double x2, double y1, double y2);
    double getHaversineDistance(double x1, double x2, double y1, double y2);
    double getEquirectangularDistance(double x1, double x2, double y1, double y2);

    /**  Arc management **/
    void setArc(int i, int j);

    //Neighbors management
    bool areNeighbors(int i, int j, bool direction) {
        if(i == j)
            return false;

        if(not direction)
            std::swap(i,j);

        return compress_data ? arcs_map[i].contains(j) : arcs[i].get(j);
    }
    std::vector<int> & getNeighbors(int node, bool direction) {return direction ? forward_neighbors[node] : backward_neighbors[node];}

    /** Active nodes management **/

    bool isActiveNode(int i) {return active_nodes.get(i);}
    void pruneActiveNode(int i) {active_nodes.set(i, false);}
    void pruneUnreachableNodes(Bitset & reachable){active_nodes &= reachable;}
    int countActiveNodes() {return active_nodes.count();}
    void activateNode(int i) {active_nodes.set(i);}
    void resetActiveNodes(){active_nodes.set();}

    /** Utilities **/
    double toRadians(double degree) {return degree * (PI / 180.0);}

private:
    static constexpr double PI = 3.14159265358979323846;

    int n_nodes;
    std::vector<int> x, y;
    Bitset active_nodes;

    bool compress_data;
    std::vector<Bitset> arcs;
    std::vector<std::map<int, bool>> arcs_map;

    std::vector<std::vector<int>> forward_neighbors, backward_neighbors;
};

#endif

