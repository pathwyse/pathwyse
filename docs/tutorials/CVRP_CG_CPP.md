# Column Generation for the CVRP (C++)

This tutorial walks through the column generation (CG) example provided under `examples/cvrp_cpp`. It shows how PathWyse can be used as a pricer inside a CG loop to solve a Capacitated Vehicle Routing Problem (CVRP) via column generation at the root node, that is, without branching.

This is the C++ port of the [Python example](CVRP_CG.md), which it mirrors line for line; see that page for the model and algorithm details. This page focuses on the C++-specific parts.

## Layout

| File | Role |
|---|---|
| `pricer.h` / `pricer.cpp` | `Pricer` — wraps a PathWyse `Solver` as an ESPPRC pricer, mirroring `columngen/pricer.py`. |
| `master.h` / `master.cpp` | `RMP` — the Restricted Master Problem, solved with the Gurobi C++ API, mirroring `columngen/master.py`. |
| `duals.h` | `Duals` — the `{mu, gamma}` pair passed from the RMP to the pricer. |
| `console.h` | Small console output helpers shared by `main.cpp` and `pricer.cpp`. |
| `main.cpp` | The CG loop, mirroring `vrp.py`. |
| `pathwyse.set` | Same algorithm configuration as the Python example. |

Unlike the Python example, which goes through the Cython wrapper (`bin/wrapper*.so`), the C++ version talks to PathWyse directly: `pricer.h` includes `solver.h` from `src/core` and links against the already-built `bin/libpathwyse_core.so`. The pricer's public methods (`updatePricers`, `solve`, `collectColumns`, ...) map 1:1 to the Python `Pricer` methods described in the [Python tutorial](CVRP_CG.md), using the [C++ API](../cpp/CPP_INTERFACES_FULL.md) (`setNodeCosts`, `setInitCost`, `resetAlgorithm`, `enable/disableAlgorithm`, `getSolutionArcCost`, `getSolutionTour`, ...) instead of their Python counterparts.

## Building

Requirements:

- `bin/libpathwyse_core.so`, built by running `./install.sh` (or the manual CMake steps) from the repository root.
- [Gurobi](https://www.gurobi.com/), with the `GUROBI_HOME` environment variable pointing at your installation (e.g. `/opt/gurobi1003/linux64`).

=== "Step by step"

    ```bash
    cd examples/cvrp_cpp
    mkdir build && cd build
    cmake ..
    make -j4
    cd ..
    ```

=== "One-liner"

    ```bash
    cd examples/cvrp_cpp && mkdir -p build && cd build && cmake .. && make -j4 && cd ..
    ```

Then, from `examples/cvrp_cpp`:

```bash
./build/column_generation_cpp <instance_path> <K>
```

`<instance_path>` is a CVRP instance in PathWyse's [instance format](../documentation/INSTANCE.md) (see `instances/spprclib` for examples), and `<K>` is the number of available vehicles.

Note that `pathwyse.set` is looked up relative to the current working directory.

### A note on the Gurobi C++ library

By default, the build links against the prebuilt `libgurobi_c++.a` shipped with Gurobi, matching Gurobi's own CMake examples. On some setups, that prebuilt archive was built against an older libstdc++ ABI and fails to link against a newer compiler, with errors such as:

```
undefined reference to `GRBModel::addVar(double, double, double, char, std::__cxx11::basic_string<...>)'
```

If you hit this, reconfigure with the wrapper rebuilt from the C++ sources Gurobi ships for this purpose (`$GUROBI_HOME/src/cpp`), instead of the prebuilt archive:

```bash
cmake .. -DGUROBI_CXX_BUILD_FROM_SOURCE=ON
make -j4
```
