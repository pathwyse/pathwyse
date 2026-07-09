# pathwyse_toy.py
from bin.wrapper import PWSolver

#Create a PathWyse solver
pw = PWSolver()

#Create Problem and set basic information
pw.createProblem()
pw.setProblemName("Toy")
pw.setNumNodes(5)
pw.setDirected(True)
pw.setCyclic(True)
pw.setOrigin(0)
pw.setDestination(4)

#Problem initialization
pw.initProblem()

#Add resources and their bounds
cap = pw.addResource("CAP")
pw.setResBounds(cap, 0, 12)

#Add arcs and their cost
arc_costs = [
    (0, 1, 54000), (0, 2, 35000), (0, 3, 54000),
    (1, 0, 54000), (1, 2, 87000), (1, 3, 75000), (1, 4, 79000),
    (2, 0, 35000), (2, 1, 87000), (2, 3, 54000),
    (3, 0, 54000), (3, 1, 75000), (3, 2, 54000), (3, 4, 98000),
    (4, 0, 45000), (4, 1, 79000), (4, 2, 59000), (4, 3, 98000),
]
for i, j, c in arc_costs:
    pw.addArc(i,j)
    pw.setArcCost(i, j, c)

#Add node costs
node_costs = {0: -7435, 1: 0, 2: -200000, 3: -7839, 4: -4000}
for node, c in node_costs.items():
    pw.setNodeCost(node, c)

#Add node consumption
node_consumption = {0: 0, 1: 2, 2: 1, 3: 6, 4: 6}
for node, c in node_consumption.items():
    pw.setResNodeConsumption(cap, node, c)

#Build the problem
pw.buildProblem()

#Setup algorithms
pw.setupAlgorithms()

#Solve the problem
pw.solve()

#Print best solution
pw.printBestSolution()
