#include "master.h"

RMP::RMP(int K, int num_nodes)
    : K(K), num_nodes(num_nodes), env(), model(env) {
    model.set(GRB_IntParam_LogToConsole, 0);
}

void RMP::buildModel(double maxObj) {
    GRBVar dummy = model.addVar(0.0, GRB_INFINITY, maxObj, GRB_CONTINUOUS, "dummy");

    // Every customer must be visited at least once (dummy covers infeasibility)
    for (int i = 1; i < num_nodes - 1; ++i) {
        std::string name = "cVisited[" + std::to_string(i) + "]";
        model.addConstr(dummy >= 1.0, name);
    }

    // At most K vehicles (routes) are used
    model.addConstr(GRBLinExpr(0.0) <= double(K), "cVehicles");

    model.update();
}

void RMP::addColumn(double cost, const std::vector<int>& col) {
    pool.push_back(col);

    GRBVar z = model.addVar(0.0, GRB_INFINITY, cost, GRB_CONTINUOUS, "z" + std::to_string(pool.size()));

    // Route col visits nodes col[1..size-2]; col[0] and col.back() are the depot
    for (size_t i = 1; i + 1 < col.size(); ++i) {
        int node = col[i];
        GRBConstr con = model.getConstrByName("cVisited[" + std::to_string(node) + "]");
        model.chgCoeff(con, z, 1.0);
    }

    GRBConstr con = model.getConstrByName("cVehicles");
    model.chgCoeff(con, z, 1.0);

    model.update();
}

void RMP::solve() {
    model.optimize();
}

Duals RMP::getDuals() {
    Duals duals;
    duals.mu.assign(num_nodes, 0.0);

    for (int i = 1; i < num_nodes - 1; ++i) {
        GRBConstr con = model.getConstrByName("cVisited[" + std::to_string(i) + "]");
        duals.mu[i] = con.get(GRB_DoubleAttr_Pi);
    }

    GRBConstr con = model.getConstrByName("cVehicles");
    duals.gamma = con.get(GRB_DoubleAttr_Pi);

    return duals;
}

double RMP::getObj() {
    return model.get(GRB_DoubleAttr_ObjVal);
}

void RMP::writeModel() {
    model.write("master.mps");
}
