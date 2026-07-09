# C++ Interface

All methods are provided through the **Solver** class (`src/core/solver.h`).

```cpp
#include "solver.h"

Solver solver;
solver.readConfiguration();
```

## Return codes

Setter methods return `RETURN_OK` (1) on success and `RETURN_ERROR` (0) on failure.
The constants are defined in `src/core/utils/constants.h`.

---

## Reference

- [Problem](CPP_INTERFACES_PROBLEM.md) — reading, creating and configuring the problem (topology, objective, resources)
- [Algorithms](CPP_INTERFACES_ALGORITHMS.md) — setup, enable/disable, reset, and solve
- [Solutions](CPP_INTERFACES_SOLUTIONS.md) — accessing, ranking and printing solutions
- [Query](CPP_INTERFACES_QUERY.md) — getters for graph, objective and resource data
