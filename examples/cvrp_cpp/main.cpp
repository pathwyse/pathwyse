#include "console.h"
#include "master.h"
#include "pricer.h"

#include <chrono>
#include <cstdlib>
#include <iostream>

static void printIterationInfo(RMP& master, Pricer& pricers, int iteration,
                                double bestRC, size_t nsols,
                                std::chrono::steady_clock::time_point start) {
    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;

    printHeader("Iteration " + std::to_string(iteration));
    printKV("Bound", master.getObj());
    printKV("Time", elapsed.count());
    printKV("Mode", pricers.isUsingHeuristics() ? "heuristic" : "exact");
    printKV("N sols", nsols);
    printKV("Best RC", bestRC);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <instance_path> <K>\n";
        return 1;
    }

    std::string data_path = argv[1];
    int K = std::atoi(argv[2]);

    Pricer pricers(data_path);
    int num_nodes = pricers.getNumNodes();
    const double max_obj = 1500000.0; // Max objective to initialize the dummy variable

    RMP master(K, num_nodes);
    master.buildModel(max_obj);

    int iteration = 0;
    const double threshold = -1e-5;
    pricers.useHeuristics();
    bool termination = false;

    auto start = std::chrono::steady_clock::now();

    std::cout << "Optimizing...\n\n";
    while (!termination) {
        master.solve();
        Duals duals = master.getDuals();
        iteration++;

        pricers.updatePricers(duals);
        pricers.solve();

        Pricer::CollectResult result = pricers.collectColumns();

        if (iteration % 20 == 0)
            printIterationInfo(master, pricers, iteration, result.best_rc, result.costs.size(), start);

        if (result.best_rc < threshold) {
            for (size_t i = 0; i < result.columns.size(); ++i)
                master.addColumn(result.costs[i], result.columns[i]);
            if (pricers.isUsingExact())
                pricers.useHeuristics();
        } else if (pricers.isUsingHeuristics()) {
            pricers.useExact();
        } else {
            termination = true;
        }
        pricers.clearColumns();
    }

    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;

    printHeader("Optimization complete");
    printKV("Objective", master.getObj());
    printKV("Iterations", iteration);
    printKV("Time", elapsed.count());

    return 0;
}
