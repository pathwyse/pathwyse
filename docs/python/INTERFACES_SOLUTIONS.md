# Solutions

Methods to access, rank and print solutions after a solve.

---

## Solution pool

#### getNumberOfSolutions

```python
def getNumberOfSolutions(self) -> int
```

Returns the number of solutions in the pool.

#### rankSolutions

```python
def rankSolutions(self, criteria)
```

Ranks the solution pool according to the given criteria.

- **criteria** *(str)* — ranking key. Use `"objective"` to sort by objective value.

#### clearSolutions

```python
def clearSolutions(self)
```

Clears the solution pool.

---

## Accessing solutions

#### getSolutionStatus

```python
def getSolutionStatus(self, sol_id) -> int
```

Returns the status of a solution.

- **sol_id** *(int)* — solution position in the pool.

#### getSolutionObjective

```python
def getSolutionObjective(self, sol_id) -> float
```

Returns the objective value of a solution.

- **sol_id** *(int)* — solution position in the pool.

#### getSolutionArcCost

```python
def getSolutionArcCost(self, sol_id) -> float
```

Returns the total arc cost of a solution.

- **sol_id** *(int)* — solution position in the pool.

#### getSolutionNodeCost

```python
def getSolutionNodeCost(self, sol_id) -> float
```

Returns the total node cost of a solution.

- **sol_id** *(int)* — solution position in the pool.

#### getSolutionTour

```python
def getSolutionTour(self, sol_id) -> list[int]
```

Returns the tour of a solution as a list of node ids.

- **sol_id** *(int)* — solution position in the pool.

---

## Output

#### printBestSolution

```python
def printBestSolution(self)
```

Prints the best solution found.

#### printAllSolutions

```python
def printAllSolutions(self)
```

Prints all solutions in the pool.

#### printNodeCosts

```python
def printNodeCosts(self)
```

Prints the cost of all nodes.
