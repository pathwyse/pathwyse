from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "solver.h":
    cdef cppclass Solver:
        Solver() except +

        #Setup
        void readProblem(string file_name)
        void setupAlgorithms() 
        void setMainAlgorithm(string name) 
        void setEnsemble(vector[string] names)
        bool isEnsembleUsed()
        void useEnsemble(bool use_ensemble)

        void resetMainAlgorithm(int level)
        void resetEnsemble(int level)

        #Optimization
        void solve()

        #Problem editing
        void setInitCost(int value)
        void setNodeCost(int id, int cost)
        void setNodeCost(vector[int] costs)

        #Solution management
        void rankSolutions(int key)
        int getNumberOfSolutions()
        int getSolutionStatus(int sol_id)
        int getSolutionObjective(int sol_id)
        int getSolutionArcCost(int sol_id)
        int getSolutionNodeCost(int sol_id)
        vector[int] getSolutionTour(int sol_id)
        void clearSolutions()

        #Query
        int getNumberOfNodes()

        #Analytics
        void printNodeCosts()
        void printBestSolution()
