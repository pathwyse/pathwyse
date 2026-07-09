# Query

Getter methods to inspect the problem and its data.

---

## Graph

#### getNumberOfNodes

```cpp
int getNumberOfNodes()
```

Returns the number of nodes.

#### getOrigin

```cpp
int getOrigin()
```

Returns the origin node id.

#### getDestination

```cpp
int getDestination()
```

Returns the destination node id.

#### isDirected

```cpp
bool isDirected()
```

Returns `true` if the graph is directed.

#### isSymmetric

```cpp
bool isSymmetric()
```

Returns `true` if the graph is symmetric.

#### isGraphCyclic

```cpp
bool isGraphCyclic()
```

Returns `true` if the graph is cyclic.

#### getName

```cpp
std::string getName()
```

Returns the problem name.

#### getProblem

```cpp
Problem* getProblem()
```

Returns a pointer to the internal `Problem` object.

---

## Objective

#### getInitCost

```cpp
double getInitCost()
```

Returns the initial cost of the problem.

#### getObjLB

```cpp
double getObjLB()
```

Returns the lower bound on the objective value.

#### getObjUB

```cpp
double getObjUB()
```

Returns the upper bound on the objective value.

#### getNodeCost

```cpp
double getNodeCost(int i)
```

Returns the cost of node `i`.

- **i** *(int)* — node id.

#### getNodeCosts

```cpp
std::vector<double> getNodeCosts()
```

Returns the costs of all nodes.

#### getArcCost

```cpp
double getArcCost(int i, int j)
```

Returns the cost of arc `(i, j)`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

---

## Resources

#### getNumRes

```cpp
int getNumRes()
```

Returns the number of resources.

#### getResLB

```cpp
int getResLB(int res_id)
```

Returns the global lower bound of a resource.

- **res_id** *(int)* — resource id.

#### getResUB

```cpp
int getResUB(int res_id)
```

Returns the global upper bound of a resource.

- **res_id** *(int)* — resource id.

#### getResNodeLB

```cpp
int getResNodeLB(int res_id, int node)
```

Returns the lower bound of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.

#### getResNodeUB

```cpp
int getResNodeUB(int res_id, int node)
```

Returns the upper bound of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.

#### getResArcConsumption

```cpp
int getResArcConsumption(int res_id, int i, int j)
```

Returns the consumption of a resource on arc `(i, j)`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

#### getResNodeConsumption

```cpp
int getResNodeConsumption(int res_id, int i)
```

Returns the consumption of a resource at node `i`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — node id.
