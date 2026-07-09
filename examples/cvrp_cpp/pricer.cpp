#include "pricer.h"
#include "console.h"
#include <cmath>

Pricer::Pricer(const std::string& instance_path)
    : using_heuristics(true), using_exact(false), reset_level(0), gamma(0.0) {
    pathwyse.readProblem(instance_path);
    pathwyse.setupAlgorithms();
    pathwyse.setAlgorithmsParallel(true);
    useHeuristics();
}

void Pricer::useHeuristics() {
    using_heuristics = true;
    using_exact = false;
    pathwyse.enableAllAlgorithms();
    pathwyse.disableAlgorithm(0);
}

void Pricer::useExact() {
    using_heuristics = false;
    using_exact = true;
    pathwyse.disableAllAlgorithms();
    pathwyse.enableAlgorithm(0);
}

void Pricer::updatePricers(const Duals& duals) {
    mu.assign(duals.mu.size(), 0.0);
    for (size_t i = 0; i < duals.mu.size(); ++i)
        mu[i] = -duals.mu[i];
    pathwyse.setNodeCosts(mu);

    gamma = -duals.gamma;
    pathwyse.setInitCost(gamma);

    for (int algo_id : pathwyse.getEnabledAlgorithms())
        pathwyse.resetAlgorithm(algo_id, reset_level);
}

void Pricer::solve() {
    pathwyse.solve();
}

// Recomputes the reduced cost of a column to avoid issues (PW obj and the
// true RC might differ, depending on scaling precision)
double Pricer::recomputeRC(double cost, const std::vector<int>& col) const {
    double rc = cost + gamma;
    for (int x : col)
        rc += mu[x];
    return rc;
}

Pricer::CollectResult Pricer::collectColumns() {
    CollectResult result;
    result.best_rc = 0.0;
    const double threshold = 0.0;

    int nsol = pathwyse.getNumberOfSolutions();
    for (int i = 0; i < nsol; ++i) {
        double pw_rc = pathwyse.getSolutionObjective(i);
        double cost = pathwyse.getSolutionArcCost(i);
        std::vector<int> col = pathwyse.getSolutionTour(i);
        double rc = recomputeRC(cost, col);
        double diff = std::abs(rc - pw_rc);

        if (debug) {
            printKV("PathWyse RC", pw_rc);
            printKV("Recomputed RC", rc);
            printKV("OBJ diff", diff);
            printKV("Column cost", cost);
            std::cout << "\n";
        }

        if (diff > 1)
            std::cout << "Warning: RC and PW objective differ by more than 1\n";

        if (rc < threshold) {
            result.costs.push_back(cost);
            result.columns.push_back(col);
        }
        if (rc < result.best_rc)
            result.best_rc = rc;
    }

    return result;
}

int Pricer::getNumNodes() {
    return pathwyse.getNumberOfNodes();
}

void Pricer::clearColumns() {
    pathwyse.clearSolutions();
}
