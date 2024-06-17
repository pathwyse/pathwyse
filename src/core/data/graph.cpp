#include "graph.h"

#include <iostream>

#include "utils/param.h"

/** Graph management **/

void Graph::initGraph(bool compress_data, int n_nodes, bool complete) {
    this->compress_data = compress_data;
    this->n_nodes = n_nodes;
    this->complete = complete;

    if(not complete) {
        if(compress_data)
            arcs_map.resize(n_nodes, std::map<int, bool>());
        else
            arcs.resize(n_nodes, Bitset(n_nodes));
    }
    forward_neighbors.resize(n_nodes, std::vector<int>());
    backward_neighbors.resize(n_nodes, std::vector<int>());
    active_nodes = Bitset(n_nodes);
    active_nodes.set();
}

/**  Coordinates and distances management **/

int Graph::getCoordDistance(int i, int j) {
    const float s = Parameters::getCoordScaling();

    const double xi = x[i]*s, yi = y[i]*s;
    const double xj = x[j]*s, yj = y[j]*s;

    double distance = 0;
    switch(Parameters::getCoordDistanceType()) {
        case DIST_EUCLIDEAN:
            distance = getEuclideanDistance(xi, xj, yi, yj);
            break;
        case DIST_HAVERSINE:
            distance = getHaversineDistance(xi, xj, yi, yj);
            break;
        case DIST_EQUIRECTANGULAR:
            distance = getEquirectangularDistance(xi, xj, yi, yj);
            break;
        default:
            break;
    }

    return static_cast<int>(floor(distance*Parameters::getCoordDistanceScaling()));
}


double Graph::getEuclideanDistance(double xi, double xj, double yi, double yj) {
    return sqrt(pow(xi - xj, 2) + pow(yi - yj, 2));
}

double Graph::getHaversineDistance(double xi, double xj, double yi, double yj) {
    const double dLon = toRadians(xj - xi);
    const double dLat = toRadians(yj - yi);

    const double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(toRadians(yi)) * cos(toRadians(yj)) *
               sin(dLon / 2) * sin(dLon / 2);

    const double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;
}

double Graph::getEquirectangularDistance(double xi, double xj, double yi, double yj) {
    const double dLon = toRadians(xj - xi) * cos(toRadians(yi + yj)/2);
    const double dLat = toRadians(yj - yi);
    return EARTH_RADIUS * sqrt(dLon * dLon + dLat * dLat);
}

/**  Arc management **/

void Graph::setArc(int i, int j) {
    if(i == j) return;

    if(not complete){
        if(compress_data)
            arcs_map[i].insert(std::make_pair(j, true));
        else
            arcs[i].set(j);
    }

    forward_neighbors[i].push_back(j);
    backward_neighbors[j].push_back(i);
}