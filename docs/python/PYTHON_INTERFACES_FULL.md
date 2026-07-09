# Python Interface

All methods are provided through the **PWSolver** class (`src/wrapper_python/wrapper.pyx`).

```python
from bin.wrapper import PWSolver
pathwyse = PWSolver()
```

## Return codes

Setter methods return `PW_OK` (1) on success and `PW_FAIL` (0) on failure:

```python
from bin.wrapper import PW_OK, PW_FAIL
```

---

## Reference

- [Problem](INTERFACES_PROBLEM.md) — reading, creating and configuring the problem (topology, objective, resources)
- [Algorithms](INTERFACES_ALGORITHMS.md) — setup, enable/disable, reset, and solve
- [Solutions](INTERFACES_SOLUTIONS.md) — accessing, ranking and printing solutions
- [Query](INTERFACES_QUERY.md) — getters for graph, objective and resource data
