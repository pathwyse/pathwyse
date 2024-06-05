from columngen.master import RMP
from columngen.pricer import SPPRCLIB
import time
import sys

def printIterationInfo(master, pricers, iteration, duals, bestRC, costs, columns, start_time):
    print("Iteration " + str(iteration))
    print("Bound: " + str(master.getObj()))
    end_time = time.time()
    print("Time: " + str(end_time - start_time))

    mode = ("heuristic" if pricers.isEnsembleUsed() else "exact")
    print("Mode: " + mode)

    print("Duals:")
    print(duals)
    print("N sols: " + str(len(costs)))
    print("Best RC: " + str(bestRC))
    print("Costs:")
    print(costs)
    print("Columns:")
    print(columns)
    print("------------------------------------")


#Setup: data_path = instance path, K = number of vehicles
data_path = sys.argv[1]
K = int(sys.argv[2]) 

pricers = SPPRCLIB(data_path)
num_nodes = pricers.getNumNodes()
max_obj = 1500000 #Max objective to initialize dummy variable

master = RMP(K, num_nodes)
master.buildModel(max_obj)

#CG
iteration = 0
threshold = -1E-5
pricers.useEnsemble(True)
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
    if iteration % 50 == 0:
        printIterationInfo(master, pricers, iteration, duals, bestRC, costs, columns, start)
    if bestRC < threshold:
        for i in range(len(columns)):
            master.addColumn(costs[i], columns[i])
        if not pricers.isEnsembleUsed():
            pricers.useEnsemble(True)
    elif pricers.isEnsembleUsed():
        pricers.useEnsemble(False)
    else:
        termination = True
    pricers.clearColumns()
end = time.time()

print("Optimization complete.")
print("Objective: " + str(master.getObj()))
print("Iterations: " + str(iteration))
print("Time: " + str(end - start))
#master.writeModel()

