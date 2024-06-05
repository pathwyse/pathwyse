#include "solver.h"

//Main file for standalone application
int main(int argc, char **argv) {

    //Read data from console
    if(not Parameters::parseConsole(argc, argv))
        return 1;

    //Create a Pathwyse solver
    Solver pathwyse = Solver();

    //Read problem
    pathwyse.readProblem();

    //Setup algorithms
    pathwyse.setupAlgorithms();

    //Solve the problem
    pathwyse.solve();

    //Print solution
    pathwyse.printBestSolution();

    return 0;
}