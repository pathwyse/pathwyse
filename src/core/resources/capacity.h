#ifndef CAPACITY_H
#define CAPACITY_H

#include "resource.h"

class Capacity: public Resource {

public:
    Capacity();
    ~Capacity() = default;

    int extend(int current_value, int i, int j, bool direction) override;
    int join(int current_value_forward, int current_value_backward, int i, int j) override;
    int join(int current_value_forward, int current_value_backward, int node) override;
    bool isFeasible(int current_value, int current_node, double bounding, bool direction) override;
};

#endif