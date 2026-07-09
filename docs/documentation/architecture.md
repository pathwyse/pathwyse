# Architecture

PathWyse is organized in four layers: entry points, data, algorithms, and utilities.

<pre>
<strong style="color:#e05252">PathWyse</strong>
├── <a href="#entry-points" style="color:#4584b6">Entry Points</a>
│   ├── Solver
│   └── PWSolver
├── <a href="#data" style="color:#4db380">Data</a>
│   ├── Problem
│   ├── Graph
│   ├── <a href="#resource-types" style="color:#3aaa8c">Resource</a>
│   │   ├── DefaultCost
│   │   ├── Capacity
│   │   ├── Time
│   │   ├── NodeLimit
│   │   └── TimeWindows
│   ├── ResourceData
│   ├── Path
│   └── BoundLabels
├── <a href="#algorithms" style="color:#e6a817">Algorithms</a>
│   ├── Algorithm
│   ├── Preprocessing
│   ├── Dijkstra
│   ├── PWDefault  ·  LMDefault
│   ├── PWBucket   ·  LMBucket
│   ├── PWAcyclic  ·  LMacyclic
│   └── Label  ·  LabelAdv
└── <a href="#utilities" style="color:#c97bdb">Utilities</a>
    ├── Parameters
    ├── DataCollector
    ├── Logger
    ├── Bitset
    └── constants
</pre>

---

## <span style="color:#4584b6">Entry Points</span>

| Class | Description |
|---|---|
| `Solver` | Main C++ interface. Owns the problem and the algorithm pipeline; exposes all setup, solve, and query operations. |
| `PWSolver` | Cython wrapper around `Solver`. Provides the same interface to Python. |

---

## <span style="color:#4db380">Data</span>

| Class | Description |
|---|---|
| `Problem` | Central data object. Holds the graph, the objective, all resources, and the current solution pool. |
| `Graph` | Adjacency structure. Stores nodes, directed arcs, and active-node bookkeeping used during preprocessing. |
| `Resource` | Base class for all resource types. Defines bounds, consumption data, and the extension interface. |
| `ResourceData` | Raw numerical data for a resource: per-node and per-arc costs, stored in dense or sparse form depending on problem size. |
| `Path` | A solution: tour sequence, objective value, and resource consumption vector. |
| `BoundLabels` | Forward and backward bound labels per node, produced by preprocessing and used to prune the search. |

### <span style="color:#3aaa8c">Resource types</span>

| Class | Description |
|---|---|
| `DefaultCost` | Objective function. Accumulates arc and node costs along the path. |
| `Capacity` | Capacity resource. Consumption is collected on nodes only. |
| `Time` | Time resource. Supports both arc travel time and node service time. |
| `NodeLimit` | Limits the number of nodes in a path. |
| `TimeWindows` | Enforces time window constraints at individual nodes. |

---

## <span style="color:#e6a817">Algorithms</span>

| Class | Description |
|---|---|
| `Algorithm` | Abstract base class for all algorithms. Defines the `solve` / `reset` interface and holds a reference to the problem. |
| `Preprocessing` | Runs Dijkstra-based rounds to compute dual bounds and prune nodes unreachable from origin or destination. |
| `Dijkstra` | Single-source shortest path used internally by `Preprocessing`. |
| `PWDefault` | Main bidirectional DP labeling algorithm. Supports DSSR, NG-path relaxations, multiple candidate and join strategies. |
| `LMDefault` | Label manager for `PWDefault`. Maintains forward and backward label pools, handles dominance checks, candidate selection, and join operations. |
| `PWBucket` | Bucket-based DP algorithm for cyclic networks. Organizes labels into resource-indexed buckets for a label-setting extension. |
| `LMBucket` | Label manager for `PWBucket`. Manages bucket structure, insertion, and dominance checks. |
| `PWAcyclic` | A\*-based algorithm for single-resource acyclic networks. |
| `LMacyclic` | Bucket-based label manager for `PWAcyclic`. |
| `Label` | Base label. Stores resource values, objective, current node, direction, and a pointer to the predecessor. |
| `LabelAdv` | Extended label with a visited-nodes bitset, required for NG-path relaxations and DSSR. |

---

## <span style="color:#c97bdb">Utilities</span>

| Class | Description |
|---|---|
| `Parameters` | Global configuration. Reads and exposes all settings from `pathwyse.set`. |
| `DataCollector` | Collects algorithm performance data (times, iterations, bounds) and writes it to disk. |
| `Logger` | Verbosity-controlled console output with color support. |
| `Bitset` | Dynamic bitset used to track visited nodes in labels efficiently. |
| `constants.h` | Global numeric constants and status codes shared across the library. |
