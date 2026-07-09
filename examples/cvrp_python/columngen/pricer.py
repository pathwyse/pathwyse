from bin.wrapper import PWSolver

def printKV(key, value):
    print("  " + key.ljust(24) + ": " + str(value))

class Pricer:
    def __init__(self, instance_path):
        self.instance_path = instance_path

        self.pathwyse = PWSolver()
        self.pathwyse.readProblem(instance_path)
        self.pathwyse.setupAlgorithms()
        self.pathwyse.setAlgorithmsParallel(True)
        self.using_heuristics = True
        self.using_exact = False
        self.reset_level = 0
        self.mu = None
        self.gamma = 0.0
        self.useHeuristics()
        self.debug = False

    def isUsingHeuristics(self):
        return self.using_heuristics
    
    def isUsingExact(self):
        return self.using_exact
    
    def useHeuristics(self):
        self.using_heuristics = True
        self.using_exact = False
        self.pathwyse.enableAllAlgorithms()
        self.pathwyse.disableAlgorithm(0)

    def useExact(self):
        self.using_heuristics = False
        self.using_exact = True
        self.pathwyse.disableAllAlgorithms()
        self.pathwyse.enableAlgorithm(0)

    def updatePricers(self, duals):
        mu = [-m for m in duals[0]]
        self.mu = mu
        self.pathwyse.setNodeCosts(mu)

        gamma = -1*duals[1]
        self.gamma = gamma
        self.pathwyse.setInitCost(gamma)
        for algo_id in self.pathwyse.getEnabledAlgorithms():
            self.pathwyse.resetAlgorithm(algo_id, self.reset_level)
        return

    def solve(self):
        self.pathwyse.solve()
        return
        
    # Recomputes the reduce costs of a column to avoid issues (PW obj and the true RC might differ, depending on scaling precision)
    def recomputeRC(self, cost, col):
        rc = cost + sum(self.mu[x] for x in col) + self.gamma
        return rc
        
    def collectColumns(self):
        threshold = 0
        bestRC = 0
        costs = list()
        columns = list()

        nsol = self.pathwyse.getNumberOfSolutions()
        for i in range(nsol):
            pw_rc = self.pathwyse.getSolutionObjective(i)
            cost = self.pathwyse.getSolutionArcCost(i)
            col = self.pathwyse.getSolutionTour(i)
            rc = self.recomputeRC(cost, col)
            diff = abs(rc - pw_rc)
            if self.debug:
                printKV("PathWyse RC", pw_rc)
                printKV("Recomputed RC", rc)
                printKV("OBJ diff", diff)
                printKV("Column cost", cost)
                print("")

            if diff > 1:
                print("Warning: RC and PW objective differ by more than 1")
            
            if rc < threshold:
                costs.append(cost)
                columns.append(col)

            if rc < bestRC:
                bestRC = rc

        return bestRC, costs, columns
    
    def getNumNodes(self):
        return self.pathwyse.getNumberOfNodes()

    def clearColumns(self):
        self.pathwyse.clearSolutions()
