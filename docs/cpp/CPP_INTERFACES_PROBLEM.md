# Problem

Methods for reading, creating and configuring the problem.

---

## Reading from file

#### readProblem

```cpp
void readProblem(std::string file_name = "")
```

Reads the problem from a file. `file_name` should normally be provided; if omitted, it falls back to the previously configured instance path (`input.txt` by default). Note that the standalone executable requires the instance path as its first command-line argument and will terminate with an error if it is missing.

- **file_name** *(std::string)* — path to the input file.

#### setCustomProblem

```cpp
void setCustomProblem(Problem& problem)
```

Sets a fully constructed external `Problem` object as the solver's problem.

- **problem** *(Problem&)* — reference to a custom Problem instance.

---

## Creating programmatically

Expected call order: `createProblem` → topology and objective setup → `initProblem` → resource setup → `buildProblem`.

#### createProblem

```cpp
void createProblem()
```

Allocates an empty problem object inside the solver.

#### setProblemName

```cpp
void setProblemName(std::string name)
```

Sets a name for the problem.

- **name** *(std::string)* — problem name.

#### initProblem

```cpp
void initProblem()
```

Initializes data structures based on the topology set so far. Must be called before adding resources.

#### buildProblem

```cpp
void buildProblem()
```

Finalizes the problem (scales costs, validates structure). Must be called after all topology, objective, and resource data have been set.

---

## Topology

Methods to define the graph structure. Must be called before `initProblem`.

#### setNumNodes

```cpp
int setNumNodes(int n)
```

Sets the number of nodes.

- **n** *(int)* — number of nodes.

#### setOrigin

```cpp
int setOrigin(int origin)
```

Sets the origin node.

- **origin** *(int)* — node id of the origin.

#### setDestination

```cpp
int setDestination(int destination)
```

Sets the destination node.

- **destination** *(int)* — node id of the destination.

#### setDirected

```cpp
int setDirected(bool directed)
```

Sets whether the graph is directed.

- **directed** *(bool)* — `true` for a directed graph.

#### setCyclic

```cpp
int setCyclic(bool cyclic)
```

Sets whether the graph is cyclic.

- **cyclic** *(bool)* — `true` for a cyclic graph.

#### setSymmetric

```cpp
int setSymmetric(bool symmetric)
```

Sets whether arc costs are the same in both directions.

- **symmetric** *(bool)* — `true` for a symmetric graph.

#### addArc

```cpp
int addArc(int i, int j)
```

Adds a directed arc from node `i` to node `j`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

---

## Objective

Methods to set the objective function costs.

#### setInitCost

```cpp
int setInitCost(double cost)
```

Sets the initial (starting) cost of the problem.

- **cost** *(double)* — initial cost.

#### setNodeCost

```cpp
int setNodeCost(int id, double cost)
```

Sets the cost of a node.

- **id** *(int)* — node id.
- **cost** *(double)* — cost value.

#### setNodeCosts

```cpp
int setNodeCosts(std::vector<double> costs)
```

Sets the cost of all nodes at once.

- **costs** *(std::vector\<double\>)* — one cost per node.

#### setArcCost

```cpp
int setArcCost(int i, int j, double cost)
```

Sets the cost of arc `(i, j)`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.
- **cost** *(double)* — arc cost.

#### setArcMatrixCosts

```cpp
void setArcMatrixCosts(std::vector<std::vector<double>> costs)
```

Sets arc costs from a full adjacency matrix.

- **costs** *(std::vector\<std::vector\<double\>\>)* — cost matrix indexed by `[i][j]`.

---

## Resources

Methods to add and configure resources. Must be called after `initProblem` and before `buildProblem`.

#### addResource

```cpp
int addResource(std::string type)
```

Adds a resource of the given type and returns its id.

- **type** *(std::string)* — resource type: `"CAP"`, `"TIME"`, `"NODELIM"`, `"TW"`.

#### setResBounds

```cpp
int setResBounds(int res_id, int lb, int ub)
```

Sets the global lower and upper bounds for a resource.

- **res_id** *(int)* — resource id.
- **lb** *(int)* — lower bound.
- **ub** *(int)* — upper bound.

#### setResNodeBound

```cpp
int setResNodeBound(int res_id, int node, int lb, int ub)
```

Sets the bounds of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.
- **lb** *(int)* — lower bound.
- **ub** *(int)* — upper bound.

#### setResArcConsumption

```cpp
int setResArcConsumption(int res_id, int i, int j, int cost)
```

Sets the consumption of a resource when traversing arc `(i, j)`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — source node id.
- **j** *(int)* — target node id.
- **cost** *(int)* — resource consumption.

#### setResNodeConsumption

```cpp
int setResNodeConsumption(int res_id, int i, int cost)
```

Sets the consumption of a resource when visiting node `i`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — node id.
- **cost** *(int)* — resource consumption.
