import sys
from bin.wrapper import PWSolver

#Read instance name from console
instance = sys.argv[1]

#Create a PathWyse solver
pw = PWSolver()

#Read problem
pw.readProblem(instance)

#Setup algorithms
pw.setupAlgorithms()

#Solve the problem
pw.solve()

#Print best solution
pw.printBestSolution()