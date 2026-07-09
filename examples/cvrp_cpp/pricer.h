#ifndef COLUMN_GENERATION_CPP_PRICER_H
#define COLUMN_GENERATION_CPP_PRICER_H

#include "solver.h"
#include "duals.h"
#include <string>
#include <vector>

// Wraps PathWyse as an ESPPRC pricer, switching between fast heuristic
// algorithms and the exact algorithm to certify optimality of the RMP.
class Pricer {
public:
    struct CollectResult {
        double best_rc;
        std::vector<double> costs;
        std::vector<std::vector<int>> columns;
    };

    explicit Pricer(const std::string& instance_path);

    bool isUsingHeuristics() const { return using_heuristics; }
    bool isUsingExact() const { return using_exact; }

    bool debug = false;

    void useHeuristics();
    void useExact();

    void updatePricers(const Duals& duals);
    void solve();

    double recomputeRC(double cost, const std::vector<int>& col) const;
    CollectResult collectColumns();

    int getNumNodes();
    void clearColumns();

private:
    Solver pathwyse;
    bool using_heuristics;
    bool using_exact;
    int reset_level;
    std::vector<double> mu;
    double gamma;
};

#endif
