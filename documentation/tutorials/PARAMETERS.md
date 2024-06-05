# Parameters

We provide a configuration file for PathWyse in the root directory: **pathwyse.set**.

If the file is present in the folder where PathWyse is run, it will override the library default configuration.

Settings are specified with the following format (case sensitive):

```
parameter = value
```
For example:
```
main_algorithm = PWDefault
```

Values can be either numbers or text. 

Please, do not add any comment to pathwyse.set.

In the following, we report the available parameters.

## Table of Contents

- [Solver parameters](#solver-parameters)
    - [Verbosity](#verbosity)
- [Problem parameters](#problem-parameters)
    - [Memory threshold](#memory-threshold)
    - [Coordinates: Distance](#coordinates-distance)
    - [Coordinates: Scaling](#coordinates-scaling)
    - [Coordinates: Microdegrees](#coordinates-microdegrees)
    - [Scaling](#scaling)
    - [Scaling: value](#scaling-value)
    - [Scaling: target](#scaling-target)
- [Algorithm parameters](#algorithm-parameters)
    - [Use ensemble](#use-ensemble) 
    - [Main algorithm](#main-algorithm)
    - [Ensemble algorithms](#ensemble-algorithms)
        - [PWDefault parameters](#pwdefault-parameters)
            - [Autoconfig](#autoconfig)
            - [Timelimit](#timelimit)
            - [Bidirectional](#bidirectional)
            - [Parallel](#parallel)
            - [Critical resource split (budget)](#critical-resource-split-budget)
            - [Label pool size](#label-pool-size)
            - [Use visited](#use-visited)
            - [Compare unreachables](#compare-unreachables)
            - [NG](#ng)
            - [NG neighbourhood size](#ng-neighbourhood-size)
            - [DSSR](#dssr)
            - [Candidate method](#candidate-method)
            - [Join method](#join-method)
- [Data Collection and Output parameters](#data-collection-and-output-parameters)
    - [Data Collection level](#data-collection-level)
    - [Output write](#output-write)



## Solver parameters
##### Verbosity
```
solver/verbosity = value
```
Controls console output.

| Value | Effect                    |
|-------|---------------------------|
| -1    | off                       |
| 0     | minimal (bound, time)     |
| 1     | adds tour information     |
| 2     | adds resource consumption |
| 3     | prints data collection    |
| 4     | debug                     |

Default: 2

## Problem parameters
##### Memory threshold
```
problem/memory_threshold = value
```

Enables memory compression and uses data structures to better support sparse data if the number of nodes of the network is greater than the provided threshold.

Value: numerical (positive, integer)

Default: 10000
##### Coordinates: Distance type
```
problem/coordinates/distance = equirectangular
```

Decides how to compute distance between two nodes.

| Value           | Effect                                                                         |
|-----------------|--------------------------------------------------------------------------------|
| none            | no function is used                                                            |
| euclidean       | use euclidean function, for planar distances                                   |
| haversine       | use haversine function, for spherical distances                                |
| equirectangular | use equirectangular heuristic, for spherical distances (faster, approximation) |

Default: none

##### Coordinates: Distance Scaling
```
problem/coordinates/distance/scaling = value
```

Multiply computed distance by value.

Value: numerical (double)

Default: 9.6

##### Coordinates: Microdegrees
```
problem/coordinates/microdegrees = 1
```

If coordinates are provided in the form of microdegrees, scale data to support degrees.

| Value | Effect                                       |
|-------|----------------------------------------------|
| 0     | do nothing                                   |
| 1     | microdegrees are being used, perform scaling |

Default: 0

##### Scaling
```
problem/scaling/override = 0
```

Allow to scale the element specified by the "problem/scaling/target" parameter by multiplying all associated values by the parameter "problem/scaling".

| Value | Effect              |
|-------|---------------------|
| 0     | off, do nothing     |
| 1     | on, perform scaling |

Default: 0

##### Scaling value
```
problem/scaling = 100.0
```

Scaling value.

Value: numerical (double)

Default: 100.0

#### Scaling target
```
problem/scaling/target = objective
```

Scaling target.

| Value     | Effect                                                                                  |
|-----------|-----------------------------------------------------------------------------------------|
| objective | scale objective data only (es: node, arc costs)                                         |
| res0      | scale res0 data only (es: node, arc consumptions). Supports also res1, res2, and so on. |
| all       | scale all data                                                                          |

Example:
```
problem/scaling/target = res0
```

Default: objective

## Algorithm parameters

PathWyse will use either the main algorithm or an ensemble of methods to solve the problem.

##### Use ensemble
```
ensemble/active = 0
```
Chooses whether to solve the problem with the ensemble or the main algorithm.


| Value | Effect             |
|-------|--------------------|
| 0     | use main algorithm |
| 1     | use ensemble       |


Default: 0

##### Main algorithm
```
main_algorithm = value
```
Sets main algorithm for resolution.

Exact algorithms:

| Value     | Effect                                                       |
|-----------|--------------------------------------------------------------|
| PWDefault | (multi-res) DP algorithm for cyclic and acyclic networks     |
| PWAcyclic | (single-res) A* based DP algorithm for acyclic networks only |

Heuristic algorithms:

| Value               | Effect                                           |
|---------------------|--------------------------------------------------|
| PWDefaultRelaxDom   | PWDefault, dominance is relaxed                  |
| PWDefaultRelaxQueue | PWDefault, label priority queues have fixed size |

Default: PWDefault


##### Ensemble algorithms

```
ensemble/algorithms = value
```

Sets the algorithms that are used by the ensemble. The same values for the main algorithm parameters are allowed. It is possible to choose from one to M algorithms.

Example:
```
ensemble/algorithms = PWDefaultRelaxDom,PWDefaultRelaxQueue
```

That is, the solver will solve the problem with PWDefaultRelaxDom and PWDefaultRelaxQueue, sequentially.

A common setup might use an ensemble of heuristics, whilst the main algorithm is exact. This might be useful in column generation scenarios, as it is possible to switch from an ensemble based optimization round to a main algorithm one. 

### PWDefault parameters

##### Autoconfig
```
algo/default/autoconfig = value
```

Autoconfigures best settings for either cyclic or acyclic problems.


| Value | Effect                      |
|-------|-----------------------------|
| 0     | off, do nothing             |
| 1     | on, autoconfigure algorithm |

Default: 1

##### Timelimit
```
algo/default/timelimit = value
```

Enforces timelimit for PWDefault. 

When the timelimit is reached, the algorithm will terminate after the extension step is over.


| Value  | Effect                       |
|--------|------------------------------|
| 0.0    | off, no timelimit            |
| \> 0.0 | on, timelimit equal to value |

Default: 0.0


##### Bidirectional
```
algo/default/bidirectional = value
```

Employs bidirectional search.


| Value | Effect                      |
|-------|-----------------------------|
| 0     | off, monodirectional search |
| 1     | on, bidirectional search    |

Default: 1

##### Parallel
```
algo/default/parallel = value
```

Performs forward and backward extension steps in parallel.

Requires "algo/default/bidirectional = 1"


| Value | Effect                   |
|-------|--------------------------|
| 0     | off, sequential search   |
| 1     | on, bidirectional search |

Default: 1

##### Critical resource split (budget)
```
algo/default/bidirectional/split = value
```

Defines the percentage of initial split for the critical resource (forward direction) during bidirectional search.

That is, the overall budget for forward steps is computed as: **split * upper_bound(critical resource)**.

The budget for backward steps is computed as follows: **(1-split) * upper_bound(critical resource)**

Requires "algo/default/bidirectional = 1"

Value: [0.0, 1.0]

Default: 0.5

##### Label pool size
```
algo/default/reserve = value
```

Defines the maximum number of labels that can be stored (in each direction) by the algorithm. 

Space is reserved in memory beforehand, therefore PathWyse will shut down if the machine does not have enough RAM for allocation.

Note that if the number of labels generated by the algorithm is greater than the pool size, the program will crash.

value: numerical (positive, integer)

Default: 10000000

##### Use visited
```
algo/default/use_visited = value
```

Decides if labels keep track of already visited nodes.

Autoconfiguration will override this setting.

| Value | Effect                         |
|-------|--------------------------------|
| 0     | off, no tracking               |
| 1     | on,  visited nodes are tracked |

Default: 1

##### Compare unreachables
```
algo/default/compare_unreachables = value
```

Decides if unreachable nodes are compared during dominance checks

Requires algo/default/use_visited = 1. 

Autoconfiguration will override this setting.


| Value | Effect |
|-------|--------|
| 0     | no     |
| 1     | yes    |

Default: 1


##### NG
```
algo/default/ng = value
```
Decides which type of NG-path relaxation apply.

Note that NG relaxations do not guarantee an elementary solution.

| Value      | Effect                                                           |
|------------|------------------------------------------------------------------|
| off        | off, no relaxation is applied                                    |
| restricted | on, iterative, forbids local cycles not allowed by ng neighbours |
| standard   | on, standard one-shot ng-path relaxation                         |

Default: off

##### NG neighbourhood size
```
algo/default/ng/set_size = value
```

Defines the size of the neighbourhood of each node, for ng-path relaxations.

Requires "algo/default/ng = restricted" or "algo/default/ng = standard"

Value: numerical (positive, integer)

Default: 8

##### DSSR
```
algo/default/dssr = value
```

Decides which type of Decremental State Space Relaxation apply. 

Note that DSSR will provide an optimal solution, if search is completed before timelimit.

Furthermore, if both NG and DSSR are enabled, the library will perform the NG relaxation first and then, if the solution found is not elementary, apply the DSSR steps.

| Value      | Effect                        |
|------------|-------------------------------|
| off        | off, no relaxation            |
| restricted | on, forbids local cycles only |
| standard   | on, classic dssr              |


Default: standard


##### Candidate method
```
algo/default/candidate/type = value
```

Decides the technique used to choose the next open label that will be extended by the algorithm

Autoconfiguration will override this setting.

| Value | Effect                                                                                                   |
|-----|----------------------------------------------------------------------------------------------------------|
| node | extend all  open labels of a node. Then move to the next node. Better for cyclic problems.               |
| roundrobin | extend the top open label from every node in a round robin fashion. Better for sparse, acyclic problems. |


Default: node

##### Join method
```
algo/default/join/type = value
```
Decides the technique used to join labels after bidirectional search.

Requires "algo/default/bidirectional = 1"

| Value   | Effect                                                                                                                                     |
|---------|--------------------------------------------------------------------------------------------------------------------------------------------|
| naive   | join all label pairs                                                                                                                       |
| classic | join labels pairs, use incumbent to cut suboptimal comparisons                                                                             |
| ordered | order label pools and join labels, use incumbent to cut suboptimal comparisons and terminate early, if no improving solutions can be found |

Default: ordered

## Data Collection and Output parameters

#### Data Collection level
```
data_collection/level = value
```
Parameter governing the extent of data collection.

| Value | Effect                       |
|-------|------------------------------|
| -1    | off, global time only        |
| 0     | on, no performance penalties |
| 1     | on, small performance hit    |


Note: values equal or greater than 1 impact performance.

Default: -1

#### Output write
```
output/write = value
```
Decides whether collected data should be written to disk or not.

By default, files are written in the output folder.

| Value | Effect                     |
|-------|----------------------------|
| 0     | off, does not write output |
| 1     | on, writes output          |

Default: 0