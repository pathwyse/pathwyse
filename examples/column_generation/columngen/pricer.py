from bin.wrapper import PWSolver

class SPPRCLIB:
    def __init__(self, instance_path):
        self.instance_path = instance_path

        self.pathwyse = PWSolver()
        self.pathwyse.readProblem(instance_path)
        self.pathwyse.setupAlgorithms()

        self.ensemble_used = True
        self.pathwyse.useEnsemble(self.ensemble_used)
        self.reset_level = 0
        self.scaling = 100

    def updatePricers(self, duals):
        mu = [-self.scaling*m for m in duals[0]]
        self.pathwyse.setNodeCost(mu)

        gamma = -self.scaling*duals[1]
        self.pathwyse.setInitCost(gamma)
        self.pathwyse.resetEnsemble(self.reset_level) if self.ensemble_used else self.pathwyse.resetMainAlgorithm(self.reset_level)
        return

    def solve(self):
        self.pathwyse.solve()
        return

    def collectColumns(self):
        threshold = 0
        bestRC = 0
        costs = list()
        columns = list()

        nsol = self.pathwyse.getNumberOfSolutions()
        for i in range(nsol):
            obj = self.pathwyse.getSolutionObjective(i)/self.scaling
            cost = self.pathwyse.getSolutionArcCost(i)/self.scaling
            col = self.pathwyse.getSolutionTour(i)

            if obj < threshold:
                costs.append(cost)
                columns.append(col)

            if obj < bestRC:
                bestRC = obj

        return bestRC, costs, columns
    
    def useEnsemble(self, used):
        self.ensemble_used = used
        self.pathwyse.useEnsemble(used)
        return

    def isEnsembleUsed(self):
        return self.ensemble_used
    
    def getNumNodes(self):
        return self.pathwyse.getNumberOfNodes()

    def clearColumns(self):
        self.pathwyse.clearSolutions()
