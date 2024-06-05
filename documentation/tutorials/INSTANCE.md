# Instance files

This is the standard instance format for PathWyse (version 0.1).

Properties of instances can be described by defining the problem and the network with either atomic values, sequences or lists. 

Only integer values are accepted, that is, if an instance has decimal values, we suggest to scale data beforehand.

Note that there is a strict order in the instance file:
- Problem setup needs to be defined first
- Network setup is defined second

Please refer to the instances provided in the instance folder for examples.

The format for atomic values and sequences is the following:

```
KEYWORD : value
```
The format for lists is instead:
```
KEYWORD
list
END
```

## Table of contents
<!-- TOC start -->

- [Instance files](#instance-files)
    * [Problem Setup](#problem-setup)
    * [Network setup](#network-setup)
        + [Resources definition](#resources-definition)
            - [Resource types](#resource-types)
            - [Resource bounds](#resource-bounds)
            - [Resource bounds on nodes](#resource-bounds-on-nodes)
        + [Edges](#edges)
            - [Edge cost](#edge-cost)
            - [Edge consumption](#edge-consumption)
        + [Nodes](#nodes)
            - [Node cost](#node-cost)
            - [Node consumption](#node-consumption)

<!-- TOC end -->


## Problem Setup

| Keyword     | Description                                                         | Value Type | Example |   
|-------------|---------------------------------------------------------------------|------------|-------|
| NAME        | instance name                                                       | text       | An-54 | 
| COMMENT     | comment                                                             | text       | example |  
| SIZE        | number of nodes                                                     | integer    | 50    |  
| DIRECTED    | graph is directed                                                   | 0 or 1     | 1     |  
| CYCLIC      | graph presents cycles                                               | 0 or 1     | 0     |  
| ORIGIN      | source node                                                         | integers   | 0     |
| DESTINATION | destination node                                                    | integers   | 17    |
| RESOURCES   | number of resources (R)                                             | integers   | 3     |
| RES_NAMES   | sequence of numerical ids (RES_ID) for each resource. From 0 to R-1 | integers   | 0 1 2 |


Note that origin and destination are optional. 

If no origin is specified, the solver will assume that node 0 is the origin. 

If no destination is reported, PathWyse will duplicate the origin node.

Note that Resource 0 will always be the critical one in bi-directional search.

Example:
```
NAME : NY
SIZE : 264346
DIRECTED : 1
CYCLIC : 0
ORIGIN : 0
DESTINATION : 17
RESOURCES : 1
RES_NAMES : 0
```
This means that this instance has 264346 nodes, the graph is directed and acyclic. There is just one resource, and we want to find the RCSPP from node 0 to node 17.

## Network setup

### Resources definition

#### Resource types
This section describes the resource types
```
RES_TYPE	
RES_ID RES_TYPE  		
END
```
Accepted RES_ID values are specified by RES_NAMES.

Accepted RES_TYPE values:

| Keyword | Resource     | Info                                                    |
|---------|--------------|---------------------------------------------------------|
| CAP     | Capacity     | Demand is collected on nodes only                       |
| TIME    | Time         | Can include travel time (arcs) and service time (nodes) |
| NODELIM | Node limit   | Limits the length of Paths                              |
| TW      | Time Windows |                                                         |


Example:
```
RES_TYPE	
0 CAP
1 CAP
2 NODELIM
3 TW
END
```
That is:
- Resource 0 is of type capacity
- Resource 1 is also a capacity resource
- Resource 2 is a node limit
- Resource 3 enforces time windows

#### Resource bounds

This section describes the lower and upper bounds for each resource:
```
RES_BOUND	
RES_ID LB UB
END
```
Example:

```
RES_BOUND		
0 0 25
1 0 28
2 0 8
END
```

This means that:
- Resource 0 has 0 as a lower bound and 25 as an upper bound
- Resource 1 has 0 as a lower bound and 28 as an upper bound
- Resource 2 has 0 as a lower bound and 8 as an upper bound
- Resource 3 has no lower and upper bounds

#### Resource bounds on nodes

This section describes the lower and upper bounds on each nodes for resources. This is mostly useful for specifying arrival and departure times for time windows:

```
RES_NODE_BOUND
RES_ID NODE_ID LB UB
END
```
Example:

```
RES_NODE_BOUND
3 1 189 579
3 2 100 307
3 3 716 1033
3 4 503 604
...
END
```

This means that we are setting node bounds for Resource 3 (time windows, in this example). For example, node 1 expects an arrival time equal or greater than 189 and a departure time of 579.

### Edges

#### Edge cost

Describe the objective function cost for each arc that goes from node i to node j:
```
EDGE_COST 
i j COST
END
```
Example:
```
EDGE_COST
0 1 43000
0 5 54000
...
END
```
This means that the arc that goes from node 0 to node 1 (and vice versa, if the network is undirected) costs 43000, whilst arc 0-5 costs 54000, and so on.

#### Edge consumption

This list describes, for each resource, the resource consumption when traveling along arcs from node i to node j:

```
EDGE_CONSUMPTION
RES_ID i j RES_CONSUMPTION
END
```

Example:
```
EDGE_CONSUMPTION
3 2 0 75
3 2 1 35
3 3 0 72
...
END
```

This means that travelling from node 2 to node 1, consumes 35 units of Resource 3. 

### Nodes

#### Node cost

This list describes the objective function cost for each node:
```
NODE_COST
NODE_ID COST 
END
```
Example: 

```
NODE_COST
1 12000 
3 -48402
...
END
```
Meaning that node 1 will cost 12000, whilst node 3 will cost -48402.

#### Node consumption

This list describes, for each resource, the resource consumption at each node:

```
NODE_CONSUMPTION
RES_ID NODE_ID RES_CONSUMPTION
END
```

Example:
```
NODE_CONSUMPTION
0 1 5
0 2 7
...
1 1 5
1 2 1
1 3 4
...
3 1 10
3 2 40
3 3 20
...
END
```

This means that:
- Visiting node 1 consumes 5 units of Resource 0, 5 units of Resource 1 and 10 units of Resource 3.
- Visiting node 2 consumes 5 units of Resource 0, 1 unit of Resource 1 and 40 units of Resource 3.

and so on.