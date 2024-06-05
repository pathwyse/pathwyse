# Python Interface

This is a list of all the methods PathWyse provides under Python through the [**PWSolver** ](../../src/wrapper_python/wrapper.pyx) class.

We remark that our Cython wrapper needs to be imported first:
```
from bin.wrapper import PWSolver
```

## Building the solver

You can build a PWSolver object, as follows:

```
pathwyse = PWSolver()
```

## Table of Contents

<!-- TOC -->
- [Setup methods](#setup-methods)
    - [readProblem](#readproblem)
    - [setupAlgorithms](#setupalgorithms)
    - [useEnsemble](#useensemble)
    - [setMainAlgorithm](#setmainalgorithm)
    - [setEnsemble](#setensemble)
    - [resetMainAlgorithm](#resetmainalgorithm)
    - [resetEnsemble](#resetensemble)

- [Optimization methods](#optimization-methods)
    - [solve](#solve)

- [Problem editing](#problem-editing)
    - [setInitCost](#setinitcost)
    - [setNodeCost](#setnodecost)
    - [setNodeCost (all)](#setnodecost-all)

- [Solution management](#solution-management)
    - [getNumberOfSolutions](#getnumberofsolutions)
    - [getSolutionStatus](#getsolutionstatus)
    - [getSolutionObjective](#getsolutionobjective)
    - [getSolutionArcCost](#getsolutionarccost)
    - [getSolutionNodeCost](#getsolutionnodecost)
    - [getSolutionTour](#getsolutiontour)
    - [clearSolutions](#clearsolutions)

- [Query](#query)
    - [getNumberOfNodes](#getnumberofnodes)
    - [isEnsembleUsed](#isensembleused)

- [Output](#output)
    - [printNodeCosts](#printnodecosts)
    - [printBestSolution](#printbestsolution)
<!-- /TOC -->

## Setup methods

Methods to configure the Solver, the problem and algorithms.

#### readProblem

    def readProblem(self, file_name):
        self.s.readProblem(file_name.encode('utf-8'))

Reads the problem associated to the file name.

Parameter: 

file_name - path to input file (string).

#### setupAlgorithms

    def setupAlgorithms(self):
        self.s.setupAlgorithms()

Configures algorithms. Uses pathwyse.set configuration, or, if not available, a default one.

#### useEnsemble

    def useEnsemble(self, use_ensemble):
        self.s.useEnsemble(use_ensemble)

Set if optimization will be performed by Ensemble algorithms, rather than main algorithm.

Param:

use_ensemble - if True Ensemble algorithms will be called during solve(), otherwise Main algorithm will be called.

#### setMainAlgorithm

    def setMainAlgorithm(self, name):
        self.s.setMainAlgorithm(name.encode('utf-8'))

Set new main algorithm.

Param: 

name - Name of the algorithm (string).

#### setEnsemble

    def setEnsemble(self, names):
      cdef vector[string] enames = [n.encode('utf-8') for n in names]
      self.s.setEnsemble(enames)

Set ensemble algorithms.

Param:

names - List of algorithm names.


#### resetMainAlgorithm

    def resetMainAlgorithm(self, level):
        self.s.resetMainAlgorithm(level)

Resets main algorithm to an unoptimized state.

Param:

level - reset level (int). Should be equal to 0, this feature is not used in PathWyse 0.1.

#### resetEnsemble

    def resetEnsemble(self, level):
        self.s.resetEnsemble(level)

Resets all ensemble algorithms to an unoptimized state.

Param:

level - reset level (int). Should be equal to 0, this feature is not used in PathWyse 0.1.

## Optimization methods

Methods to solve the problem.

#### solve

    def solve(self):
        self.s.solve()

Solves the problem.

#Problem editing

Methods to edit an already defined problem.

#### setInitCost

    def setInitCost(self, value):
        self.s.setInitCost(value)

Changes the starting cost of the problem.

#### setNodeCost

    def setNodeCost(self, id, cost):
        self.s.setNodeCost(id, cost)

Change cost of a node.

Param:

id - Id of the node (int).

cost - Cost to be assigned to the node (int).


#### setNodeCost (all)

    def setNodeCost(self, costs):
        cdef vector[int] vect = costs
        self.s.setNodeCost(costs)

Change cost of all nodes.

Param:

costs (list of ints) - cost of the nodes. Requires one cost for each node.

## Solution management

#### getNumberOfSolutions

    def getNumberOfSolutions(self):
        return self.s.getNumberOfSolutions()

Returns the number of solutions.

#### getSolutionStatus

    def getSolutionStatus(self, sol_id):
        return self.s.getSolutionStatus(sol_id)

Returns the status of a solution.

Param:

sol_id - solution position (int).

#### getSolutionObjective

    def getSolutionObjective(self, id):
        return self.s.getSolutionObjective(id)

Returns the objective value of a solution.

Param:

id - solution position (int).

#### getSolutionArcCost

    def getSolutionArcCost(self, id):
        return self.s.getSolutionArcCost(id)

Returns the total cost of the arcs along a solution path.

Param:

id - solution position (int).

#### getSolutionArcCost

    def getSolutionNodeCost(self, id):
        return self.s.getSolutionNodeCost(id)

Returns the total cost of the nodes along a solution path.

Param:

id - solution position (int).

#### getSolutionTour


    def getSolutionTour(self, id):
        cdef vector[int] vtour = self.s.getSolutionTour(id)
        return list(vtour)

Returns the tour of a solution path as a list.

Param:

id - solution position (int).

#### clearSolutions


    def clearSolutions(self):
        self.s.clearSolutions()

Clears the solution pool.

## Query

#### getNumberOfNodes

     def getNumberOfNodes(self):
        return self.s.getNumberOfNodes()

Returns the number of nodes.

#### isEnsembleUsed

    def isEnsembleUsed(self):
        return self.s.isEnsembleUsed()

Returns True if Ensemble is currently used for optimization. Returns False if Main Algorithm is used instead. 

## Output

#### printNodeCosts

    def printNodeCosts(self):
        self.s.printNodeCosts()

Prints the current cost of all nodes.

#### printBestSolution

    def printBestSolution(self):
        self.s.printBestSolution()

Prints the best solution found.

