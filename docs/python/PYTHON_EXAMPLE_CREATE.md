# Creating an instance programmatically

PathWyse can build and solve a problem entirely from Python, without an instance file. The full example is at `examples/python_application/pathwyse_instance_creation.py`. The walkthrough below creates a small 5-node RCSPP with a capacity resource.

Step 1) Import PathWyse wrapper and create a solver

```python
from bin.wrapper import PWSolver
pw = PWSolver()
```

Step 2) Create a problem and set its basic properties

```python
pw.createProblem()
pw.setProblemName("Toy")
pw.setNumNodes(5)
pw.setDirected(True)
pw.setCyclic(True)
pw.setOrigin(0)
pw.setDestination(4)
```

Step 3) Initialize internal data structures

```python
pw.initProblem()
```

This must be called before adding resources.

Step 4) Add resources and set their global bounds

```python
cap = pw.addResource("CAP")
pw.setResBounds(cap, 0, 12)
```

Step 5) Add arcs and their costs

```python
arc_costs = [
    (0, 1, 54000), (0, 2, 35000), (0, 3, 54000),
    (1, 0, 54000), (1, 2, 87000), (1, 3, 75000), (1, 4, 79000),
    (2, 0, 35000), (2, 1, 87000), (2, 3, 54000),
    (3, 0, 54000), (3, 1, 75000), (3, 2, 54000), (3, 4, 98000),
    (4, 0, 45000), (4, 1, 79000), (4, 2, 59000), (4, 3, 98000),
]
for i, j, c in arc_costs:
    pw.addArc(i, j)
    pw.setArcCost(i, j, c)
```

Step 6) Set node costs

```python
node_costs = {0: -7435, 1: 0, 2: -200000, 3: -7839, 4: -4000}
for node, c in node_costs.items():
    pw.setNodeCost(node, c)
```

Step 7) Set node resource consumptions

```python
node_consumption = {0: 0, 1: 2, 2: 1, 3: 6, 4: 6}
for node, c in node_consumption.items():
    pw.setResNodeConsumption(cap, node, c)
```

Step 8) Build the problem

```python
pw.buildProblem()
```

This finalizes the problem (scales costs, validates structure) and must be called after all data has been set.

Step 9) Setup algorithms, solve, and print

```python
pw.setupAlgorithms()
pw.solve()
pw.printBestSolution()
```
