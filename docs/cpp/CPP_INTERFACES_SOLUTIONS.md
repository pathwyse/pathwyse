# Solutions

Methods to access, rank and print solutions after a solve.

---

## Solution pool

#### getNumberOfSolutions

```cpp
int getNumberOfSolutions()
```

Returns the number of solutions in the pool.

#### rankSolutions

```cpp
void rankSolutions(std::string criteria = "objective")
```

Ranks the solution pool according to the given criteria.

- **criteria** *(std::string)* — ranking key. Use `"objective"` to sort by objective value.

#### clearSolutions

```cpp
void clearSolutions()
```

Clears the solution pool.

#### getAllSolutions

```cpp
std::vector<Path> getAllSolutions()
```

Returns all solutions in the pool.

#### getBestSolutions

```cpp
std::vector<Path> getBestSolutions(int pool_size = 1)
```

Returns the best solutions in the pool.

- **pool_size** *(int)* — number of solutions to return.

#### getBestSolution

```cpp
Path getBestSolution()
```

Returns the best solution found.

---

## Accessing solutions

#### getSolution

```cpp
Path* getSolution(int id)
```

Returns a pointer to the solution at position `id` in the pool.

- **id** *(int)* — solution position in the pool.

#### getSolutionStatus

```cpp
int getSolutionStatus(int id)
```

Returns the status of a solution.

- **id** *(int)* — solution position in the pool.

#### getSolutionObjective

```cpp
double getSolutionObjective(int id)
```

Returns the objective value of a solution.

- **id** *(int)* — solution position in the pool.

#### getSolutionArcCost

```cpp
double getSolutionArcCost(int id)
```

Returns the total arc cost of a solution.

- **id** *(int)* — solution position in the pool.

#### getSolutionNodeCost

```cpp
double getSolutionNodeCost(int id)
```

Returns the total node cost of a solution.

- **id** *(int)* — solution position in the pool.

#### getSolutionTour

```cpp
std::vector<int> getSolutionTour(int id)
```

Returns the tour of a solution as a list of node ids.

- **id** *(int)* — solution position in the pool.

#### getSolutionTourAsString

```cpp
std::string getSolutionTourAsString(int id)
```

Returns the tour of a solution as a space-separated string of node ids.

- **id** *(int)* — solution position in the pool.

---

## Output

#### printStatus

```cpp
void printStatus()
```

Prints the current solver status.

#### printBestSolution

```cpp
void printBestSolution()
```

Prints the best solution found.

#### printAllSolutions

```cpp
void printAllSolutions()
```

Prints all solutions in the pool.

#### printAlgorithmsStatus

```cpp
void printAlgorithmsStatus()
```

Prints the status of all algorithms.

#### printNodeCosts

```cpp
void printNodeCosts()
```

Prints the cost of all nodes.
