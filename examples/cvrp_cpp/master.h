#ifndef COLUMN_GENERATION_CPP_MASTER_H
#define COLUMN_GENERATION_CPP_MASTER_H

#include "gurobi_c++.h"
#include "duals.h"
#include <string>
#include <vector>

// Restricted Master Problem: minimize the cost of the routes selected to
// cover every customer, using at most K vehicles.
class RMP {
public:
    RMP(int K, int num_nodes);

    void buildModel(double maxObj);
    void addColumn(double cost, const std::vector<int>& col);

    void solve();
    Duals getDuals();
    double getObj();

    void writeModel();

private:
    int K;
    int num_nodes;
    GRBEnv env;
    GRBModel model;
    std::vector<std::vector<int>> pool;
};

#endif
