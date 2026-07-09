from columngen.master import RMP
from columngen.pricer import Pricer
import time
import sys

# Console output helpers
WIDTH = 44

def printHeader(title):
    print("-" * WIDTH)
    print("[ " + title + " ]")
    print("-" * WIDTH)

def printKV(key, value):
    print("  " + key.ljust(24) + ": " + str(value))

def printIterationInfo(master, pricers, iteration, bestRC, nsols, start_time):
    printHeader("Iteration " + str(iteration))
    printKV("Bound", master.getObj())
    printKV("Time", "%.3f" % (time.time() - start_time))
    printKV("Mode", "heuristic" if pricers.isUsingHeuristics() else "exact")
    printKV("N sols", nsols)
    printKV("Best RC", bestRC)


#Setup: data_path = instance path, K = number of vehicles
data_path = sys.argv[1]
K = int(sys.argv[2]) 

pricers = Pricer(data_path)
num_nodes = pricers.getNumNodes()
max_obj = 1500000 #Max objective to initialize dummy variable

master = RMP(K, num_nodes)
master.buildModel(max_obj)

#CG
iteration = 0
threshold = -1E-5
pricers.useHeuristics()
termination = False

start = time.time()

print("Optimizing...\n")
while(not termination):
    #RMP solve and obtain new duals
    master.solve()
    duals = master.getDuals()
    iteration += 1


    #Update pricers and solve them
    pricers.updatePricers(duals) 
    pricers.solve() 

    #Collect columns and update RMP
    bestRC, costs, columns = pricers.collectColumns()
    if iteration % 20 == 0:
        printIterationInfo(master, pricers, iteration, bestRC, len(costs), start)
    if bestRC < threshold:
        for i in range(len(columns)):
            master.addColumn(costs[i], columns[i])
        if pricers.isUsingExact():
            pricers.useHeuristics()
    elif pricers.isUsingHeuristics():
        pricers.useExact()
    else:
        termination = True
    pricers.clearColumns()
end = time.time()

printHeader("Optimization complete")
printKV("Objective", master.getObj())
printKV("Iterations", iteration)
printKV("Time", "%.3f" % (end - start))
#master.writeModel()

