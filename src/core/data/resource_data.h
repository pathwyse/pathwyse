#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include <vector>
#include <map>
#include <cmath>
#include "utils/constants.h"

struct ResourceData {

    /** Resource data management **/
    //Constructors and Destructors
    explicit ResourceData(int n_nodes) {this->n_nodes = n_nodes; coord_distance_type = DIST_NONE;}
    virtual ~ResourceData() = default;

    virtual void reset() {node_costs.clear(); node_costs_d.clear(); node_max_abs = 0.0; arc_max_abs = 0.0; node_max_dirty = false; arc_max_dirty = false;};

    /** Node Data **/
    void setNodeCosts(std::vector<int> node_costs){this->node_costs = node_costs;}
    void setNodeCost(int id, int cost){
        if(node_costs.empty())
            node_costs.resize(n_nodes, 0);
        node_costs[id] = cost;
    }

    int getNodeCost(int id){return node_costs.empty() ? 0: node_costs[id];}
    std::vector<int> & getNodeCosts() {return node_costs;}

    /** Arc Data **/
    virtual void setArcCost(int i, int j, int cost) = 0;
    virtual int getArcCost(int i, int j) = 0;

    /** Distance Type **/
    int getCoordDistanceType(){return coord_distance_type;}
    void setCoordDistanceType(int type){coord_distance_type = type;}

    /** Double representation (costs stored as double, converted to int before solve) **/
    void setNodeCostDouble(int id, double cost) {
        if (node_costs_d.empty()) node_costs_d.resize(n_nodes, 0.0);
        node_costs_d[id] = cost;
        node_max_dirty = true;
    }
    double getNodeCostDouble(int id) { return node_costs_d.empty() ? 0.0 : node_costs_d[id]; }
    const std::vector<double>& getNodeCostsDouble() { if (node_costs_d.empty()) node_costs_d.resize(n_nodes, 0.0); return node_costs_d; }
    double getMaxAbsValue() {
        if (node_max_dirty) recomputeNodeMax();
        if (arc_max_dirty) recomputeArcMax();
        return std::max(node_max_abs, arc_max_abs);
    }

    virtual void setArcCostDouble(int i, int j, double cost) = 0;
    virtual double getArcCostDouble(int i, int j) = 0;
    virtual void applyScale(double scale) = 0;

protected:
    int n_nodes;
    std::vector<int> node_costs;
    int coord_distance_type;
    std::vector<double> node_costs_d;

    double node_max_abs = 0.0;
    double arc_max_abs = 0.0;
    bool node_max_dirty = false;
    bool arc_max_dirty = false;

    void recomputeNodeMax() {
        node_max_abs = 0.0;
        for (double v : node_costs_d) { double a = std::abs(v); if (a > node_max_abs) node_max_abs = a; }
        node_max_dirty = false;
    }
    virtual void recomputeArcMax() = 0;
};

struct ResourceDataMap: ResourceData {

    /** Resource data management **/
    //Constructors and Destructors
    explicit ResourceDataMap(int n_nodes): ResourceData(n_nodes) {
        arc_costs.resize(n_nodes, std::map<int, int>());
    }
    ~ResourceDataMap() = default;

    void reset() override {ResourceData::reset(); arc_costs.clear(); arc_costs_d.clear();}

    /** Arc Data **/
    void setArcCost(int i, int j, int cost) override {
        auto position = arc_costs[i].find(j);
        if(position == arc_costs[i].end())
            arc_costs[i].insert(std::make_pair(j, cost));
        else
            position->second = cost;
    }

    int getArcCost(int i, int j) override {
        auto position = arc_costs[i].find(j);
        return position != arc_costs[i].end()? position->second : 0;
    }

    /** Double representation **/
    void setArcCostDouble(int i, int j, double cost) override {
        if (arc_costs_d.empty()) arc_costs_d.resize(n_nodes);
        double abs_cost = std::abs(cost);
        double old_abs = std::abs(arc_costs_d[i][j]);
        arc_costs_d[i][j] = cost;
        if (abs_cost > arc_max_abs) arc_max_abs = abs_cost;
        else if (old_abs >= arc_max_abs and abs_cost < arc_max_abs) arc_max_dirty = true;
    }
    double getArcCostDouble(int i, int j) override {
        if (arc_costs_d.empty()) return 0.0;
        auto it = arc_costs_d[i].find(j);
        return it != arc_costs_d[i].end() ? it->second : 0.0;
    }
    void applyScale(double scale) override {
        for (int i = 0; i < n_nodes; i++)
            for (auto& [j, c] : arc_costs_d[i])
                setArcCost(i, j, (int)(c * scale));
        for (int i = 0; i < (int)node_costs_d.size(); i++)
            setNodeCost(i, (int)(node_costs_d[i] * scale));
    }

protected:
    void recomputeArcMax() override {
        arc_max_abs = 0.0;
        for (int i = 0; i < n_nodes; i++)
            for (auto& [j, c] : arc_costs_d[i]) { double abs_cost = std::abs(c); if (abs_cost > arc_max_abs) arc_max_abs = abs_cost; }
        arc_max_dirty = false;
    }

private:
    std::vector<std::map<int, int>> arc_costs;
    std::vector<std::map<int, double>> arc_costs_d;

};

struct ResourceDataMatrix: ResourceData {

    /** Resource data management **/
    //Constructors and Destructors
    explicit ResourceDataMatrix(int n_nodes): ResourceData(n_nodes) {
        arc_costs.resize(n_nodes, std::vector<int>(n_nodes, 0));
    }
    ~ResourceDataMatrix() = default;

    void reset() override {ResourceData::reset(); arc_costs.clear(); arc_costs_d.clear();}

    /** Arc Data **/
    void setArcCost(int i, int j, int cost) override {arc_costs[i][j] = cost;}
    int getArcCost(int i, int j) override {return arc_costs[i][j];}

    /** Double representation **/
    void setArcCostDouble(int i, int j, double cost) override {
        if (arc_costs_d.empty()) arc_costs_d.assign(n_nodes, std::vector<double>(n_nodes, 0.0));
        double abs_cost = std::abs(cost);
        if (abs_cost > arc_max_abs) arc_max_abs = abs_cost;
        else if (std::abs(arc_costs_d[i][j]) >= arc_max_abs and abs_cost < arc_max_abs) arc_max_dirty = true;
        arc_costs_d[i][j] = cost;
    }
    double getArcCostDouble(int i, int j) override {
        return arc_costs_d.empty() ? 0.0 : arc_costs_d[i][j];
    }
    void applyScale(double scale) override {
        if (arc_costs_d.empty()) return;
        for (int i = 0; i < n_nodes; i++)
            for (int j = 0; j < n_nodes; j++)
                arc_costs[i][j] = (int)(arc_costs_d[i][j] * scale);
        for (int i = 0; i < (int)node_costs_d.size(); i++)
            setNodeCost(i, (int)(node_costs_d[i] * scale));
    }

protected:
    void recomputeArcMax() override {
        arc_max_abs = 0.0;
        for (int i = 0; i < n_nodes; i++)
            for (int j = 0; j < n_nodes; j++) { double abs_cost = std::abs(arc_costs_d[i][j]); if (abs_cost > arc_max_abs) arc_max_abs = abs_cost; }
        arc_max_dirty = false;
    }

private:
    std::vector<std::vector<int>> arc_costs;
    std::vector<std::vector<double>> arc_costs_d;
};

#endif
