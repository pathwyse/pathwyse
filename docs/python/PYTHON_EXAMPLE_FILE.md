# Solving from an instance file

A full example can be found at `examples/python_application/pathwyse.py`.

Import the wrapper, read the instance path from the command line, and build the solver:

```python
import sys
from bin.wrapper import PWSolver

instance = sys.argv[1]
pathwyse = PWSolver()
```

Read the problem, configure algorithms, solve, and print the best solution:

```python
pathwyse.readProblem(instance)
pathwyse.setupAlgorithms()
pathwyse.solve()
pathwyse.printBestSolution()
```
