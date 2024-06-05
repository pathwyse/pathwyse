#ifndef DEFAULTCOST_H
#define DEFAULTCOST_H

#include "resource.h"

class DefaultCost: public Resource {

public:
    DefaultCost();
    ~DefaultCost() = default;

    int extend(int current_value, int i, int j, bool direction) override;
    int join(int current_value_forward, int current_value_backward, int i, int j) override;
    int join(int current_value_forward, int current_value_backward, int node) override;
    bool isFeasible(int current_value, int current_node, double bounding, bool direction) override;
};


#endif