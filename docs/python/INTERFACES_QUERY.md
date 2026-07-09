# Query

Getter methods to inspect the problem and its data.

---

## Graph

#### getNumberOfNodes

```python
def getNumberOfNodes(self) -> int
```

Returns the number of nodes.

#### getOrigin

```python
def getOrigin(self) -> int
```

Returns the origin node id.

#### getDestination

```python
def getDestination(self) -> int
```

Returns the destination node id.

#### isDirected

```python
def isDirected(self) -> bool
```

Returns `True` if the graph is directed.

#### isSymmetric

```python
def isSymmetric(self) -> bool
```

Returns `True` if the graph is symmetric.

#### isGraphCyclic

```python
def isGraphCyclic(self) -> bool
```

Returns `True` if the graph is cyclic.

#### getName

```python
def getName(self) -> str
```

Returns the problem name.

---

## Objective

#### getInitCost

```python
def getInitCost(self) -> float
```

Returns the initial cost of the problem.

#### getObjLB

```python
def getObjLB(self) -> float
```

Returns the lower bound on the objective value.

#### getObjUB

```python
def getObjUB(self) -> float
```

Returns the upper bound on the objective value.

#### getNodeCost

```python
def getNodeCost(self, i) -> float
```

Returns the cost of node `i`.

- **i** *(int)* — node id.

#### getNodeCosts

```python
def getNodeCosts(self) -> list[float]
```

Returns the costs of all nodes.

#### getArcCost

```python
def getArcCost(self, i, j) -> float
```

Returns the cost of arc `(i, j)`.

- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

---

## Resources

#### getNumRes

```python
def getNumRes(self) -> int
```

Returns the number of resources.

#### getResLB

```python
def getResLB(self, res_id) -> int
```

Returns the global lower bound of a resource.

- **res_id** *(int)* — resource id.

#### getResUB

```python
def getResUB(self, res_id) -> int
```

Returns the global upper bound of a resource.

- **res_id** *(int)* — resource id.

#### getResNodeLB

```python
def getResNodeLB(self, res_id, node) -> int
```

Returns the lower bound of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.

#### getResNodeUB

```python
def getResNodeUB(self, res_id, node) -> int
```

Returns the upper bound of a resource at a specific node.

- **res_id** *(int)* — resource id.
- **node** *(int)* — node id.

#### getResArcConsumption

```python
def getResArcConsumption(self, res_id, i, j) -> int
```

Returns the consumption of a resource on arc `(i, j)`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — source node id.
- **j** *(int)* — target node id.

#### getResNodeConsumption

```python
def getResNodeConsumption(self, res_id, i) -> int
```

Returns the consumption of a resource at node `i`.

- **res_id** *(int)* — resource id.
- **i** *(int)* — node id.
