# Python

## Getting started in Python

Python interfaces can be used by importing the Cython wrapper. 

Please, place (or copy) the bin folder one level below your python file. 

You can then import the wrapper like this:

```
from bin.wrapper import PWSolver
```

The interface to the library is provided by the PWSolver class. To build it:

```
pathwyse = PWSolver()
```

A list of all the methods provided by PWSolver can be found [here](PYTHON_INTERFACES.md).

## Python application example 

An example of a full PathWyse execution can be found in the example folder. It is called "python_application".


Step 1)
Import PathWyse wrapper

```
import sys
from bin.wrapper import PWSolver
```
Step 2) Read instance name from console

```
instance = sys.argv[1]
```
Step 3) Create a PathWyse solver by calling the constructor
```
pathwyse = PWSolver()
```

Step 4) Read the problem
```
pathwyse.readProblem(instance)
```
Step 5) Setup algorithms for optimization.
```
pathwyse.setupAlgorithms()
```
Step 6) Solve the problem
```
pathwyse.solve()
```
Step 7) Print the best solution found
```
pathwyse.printBestSolution()
```