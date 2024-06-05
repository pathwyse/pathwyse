# Extras

## Overriding instance configuration

PathWyse provides few command lines keywords to override instance data on the fly, in a terminal:

```
./bin/pathwyse path-to-instance -keyword value
```

The allowed keywords are the following:

| Keyword | Effect                                     | Value Type |
|---------|--------------------------------------------|------------|
| -o      | overrides instance origin                  | integer    |
| -d      | overrides instance destination             | integer    |
| -ud     | overrides upper bound of critical resource | integer    |

For example:

```
./bin/pathwyse instances/spprclib/A-n54-k7-149.sppcc -o 20 -d 50 -ub 200
```

Will solve the RCSPP from node 20 to node 50, with a budget of 200 for the critical resource.

## Instance examples

We provide a toy instance called "toy.txt" in the instance folder. It is a very simple problem to test that the library works.

Then, we provide few examples of instances from the literature  converted to PathWyse format.
- SPPRCLIB [Jepsen et al., 2008]
- DIMACS, from DIMACS 9th Challenge
- LongestPath [Salani et al., 2024]

## A Column Generation Example

A column generation example for solving VRP problems at the root node, that is tailored for SPPRCLIB instances, is provided under examples/column generation. 

In this example, PathWyse is used by the pricer, whilst the master problem is solved by Gurobi.
