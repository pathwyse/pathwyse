#Python Wrapper
from libcpp.vector cimport vector

cdef class PWSolver:
    cdef Solver* s

    #Constructor
    def __cinit__(self):
        self.s = new Solver()

    def __dealloc__(self):
        del self.s

    #Setup
    def readProblem(self, file_name):
        self.s.readProblem(file_name.encode('utf-8'))

    def setupAlgorithms(self):
        self.s.setupAlgorithms()

    def setMainAlgorithm(self, name):
        self.s.setMainAlgorithm(name.encode('utf-8'))

    def setEnsemble(self, names):
        cdef vector[string] enames = [n.encode('utf-8') for n in names]
        self.s.setEnsemble(enames)

    def isEnsembleUsed(self):
        return self.s.isEnsembleUsed()

    def useEnsemble(self, use_ensemble):
        self.s.useEnsemble(use_ensemble)

    def resetMainAlgorithm(self, level):
        self.s.resetMainAlgorithm(level)

    def resetEnsemble(self, level):
        self.s.resetEnsemble(level)

    #Optimization
    def solve(self):
        self.s.solve()

    #Problem editing
    def setInitCost(self, value):
        self.s.setInitCost(value)

    def setNodeCost(self, id, cost):
        self.s.setNodeCost(id, cost)

    def setNodeCost(self, costs):
        cdef vector[int] vect = costs
        self.s.setNodeCost(costs)

    #Solution management
    def getNumberOfSolutions(self):
        return self.s.getNumberOfSolutions()

    def getSolutionStatus(self, sol_id):
        return self.s.getSolutionStatus(sol_id)

    def getSolutionObjective(self, id):
        return self.s.getSolutionObjective(id)

    def getSolutionArcCost(self, id):
        return self.s.getSolutionArcCost(id)

    def getSolutionNodeCost(self, id):
        return self.s.getSolutionNodeCost(id)

    def getSolutionTour(self, id):
        cdef vector[int] vtour = self.s.getSolutionTour(id)
        return list(vtour)

    def clearSolutions(self):
        self.s.clearSolutions()

    #Query
    def getNumberOfNodes(self):
        return self.s.getNumberOfNodes()

    #Analytics
    def printNodeCosts(self):
        self.s.printNodeCosts()

    def printBestSolution(self):
        self.s.printBestSolution()

