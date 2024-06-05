#ifndef BOUND_DATA_H
#define BOUND_DATA_H

#include "algorithms/labels/label.h"

struct BoundLabels {

    /** Bound Labels management **/

    //Constructors and destructors
    BoundLabels(int n_res, int n_nodes) {
        this->n_res = n_res;
        this->n_nodes = n_nodes;
        initData();
    }
    ~BoundLabels() = default;

    void initData(){
        fw_cost.resize(n_nodes, Label());
        bw_cost.resize(n_nodes, Label());
        fw_consumption.resize(n_res, std::vector<Label>(n_nodes, Label()));
        bw_consumption.resize(n_res, std::vector<Label>(n_nodes, Label()));
    }

    void resetData(){
        fw_cost.clear();
        bw_cost.clear();
        fw_consumption.clear();
        bw_consumption.clear();
        initData();
    }

    //Getters
    Label* getLabel(int res_id, bool direction, int node) {
        if(res_id == RES_COST) {
            auto &cost = direction ? fw_cost : bw_cost;
            return & cost[node];
        }
        else {
            auto &consumption = direction ? fw_consumption: bw_consumption;
            return & consumption[res_id][node];
        }
    }

    std::vector<Label>* getCostLabels(int direction){return direction? & fw_cost: & bw_cost;}
    std::vector<Label>* getConsumptionLabels(int res_id, int direction){return direction? & fw_consumption[res_id]: & bw_consumption[res_id];}

private:

    int n_res;
    int n_nodes;
    std::vector<Label> fw_cost, bw_cost;
    std::vector<std::vector<Label>> fw_consumption, bw_consumption;
};


#endif