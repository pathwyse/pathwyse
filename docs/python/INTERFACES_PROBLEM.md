# Problem

Methods for reading, creating and configuring the problem.

---

## Reading from file

#### readProblem

```python
def readProblem(self, file_name)
```

Reads the problem from a file.

- **file_name** *(str)* — path to the input file.

---

## Creating programmatically

Expected call order: `createProblem` → topology and objective setup → `initProblem` → resource setup → `buildProblem`.

#### createProblem

```python
def createProblem(self)
```

Allocates an empty problem object inside the solver.

#### setProblemName

```python
def setProblemName(self, name)
```

Sets a name for the problem.

- **name** *(str)* — problem name.

#### initProblem

```python
def initProblem(self)
```

Initializes data structures based on the topology set so far. Must be called before adding resources.

#### buildProblem

```python
def buildProblem(self)
```

Finalizes the problem (scales costs, validates structure). Must be called after all topology, objective, and resource data have been set.

---

## Topology

Methods to define the graph structure. Must be called before `initProblem`.

#### setNumNodes

```python
def setNumNodes(self, n)
```

Sets the number of nodes.

- **n** *(int)* — number of nodes.

#### setOrigin

```python
def setOrigin(self, origin)
```

Sets the origin node.

- **origin** *(int)* — node id of the origin.

#### setDestination

```python
def setDestination(self, destination)
```

Sets the destination node.

- **destination** *(int)* — node id of the destination.

#### setDirected

```python
def setDirected(self, directed)
```

Sets whether the graph is directed.

- **directed** *(bool)* — `True` for a directed graph.

#### setCyclic

```python
def setCyclic(self, cyclic)
```

Sets whether the graph is cyclic.

- **cyclic** *(bool)* — `True` for a cyclic graph.

#### setSymmetric

```python
def setSymmetric(self, symmetric)
```

Sets whether arc costs are the same in both directions.

- **symmetric** *(bool)* — `True` for a symmetric graph.

#### addArc

```python
def addArc(self, i, j)
```

Adds a directed arc from node `i` to node `j`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

---

## Objective

Methods to set the objective function costs.

#### setInitCost

```python
def setInitCost(self, value)
```

Sets the initial (starting) cost of the problem.

- **value** *(float)* — initial cost.

#### setNodeCost

```python
def setNodeCost(self, id, cost)
```

Sets the cost of a node.

- **id** *(int)* — node id.
- **cost** *(float)* — cost value.

#### setNodeCosts

```python
def setNodeCosts(self, costs)
```

Sets the cost of all nodes at once.

- **costs** *(list[float])* — one cost per node.

#### setArcCost

```python
def setArcCost(self, i, j, cost)
```

Sets the cost of arc `(i, j)`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.
- **cost** *(float)* — arc cost.

---

## Resources

Methods to add and configure resources. Must be called after `initProblem` and before `buildProblem`.

#### addResource

```python
def addResource(self, res_type) -> int
```

Adds a resource of the given type and returns its id.

- **res_type** *(str)* — resource type: `"CAP"`, `"TIME"`, `"NODELIM"`, `"TW"`.

#### setResBounds

```python
def setResBounds(self, res_id, lb, ub)
```

Sets the global lower and upper bounds for a resource.

- **res_id** *(int)* — resource id.
- **lb** *(int)* — lower bound.
- **ub** *(int)* — upper bound.

#### setResNodeBound

```python
def setResNodeBound(self, res_id, node, lb, ub)
```

Sets the bounds of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.
- **lb** *(int)* — lower bound.
- **ub** *(int)* — upper bound.

#### setResArcConsumption

```python
def setResArcConsumption(self, res_id, i, j, cost)
```

Sets the consumption of a resource when traversing arc `(i, j)`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — source node id.
- **j** *(int)* — target node id.
- **cost** *(int)* — resource consumption.

#### setResNodeConsumption

```python
def setResNodeConsumption(self, res_id, i, cost)
```

Sets the consumption of a resource when visiting node `i`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — node id.
- **cost** *(int)* — resource consumption.
