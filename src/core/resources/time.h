#ifndef TIME_H
#define TIME_H

#include "resource.h"

class Time: public Resource {

public:
    Time();
    ~Time() = default;

    int extend(int current_value, int i, int j, bool direction) override;
    int join(int current_value_forward, int current_value_backward, int i, int j) override;
    int join(int current_value_forward, int current_value_backward, int node) override;
    bool isFeasible(int current_value, int current_node, double bounding, bool direction) override;
};

#endif
