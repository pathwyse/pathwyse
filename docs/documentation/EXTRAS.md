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
| -ub     | overrides upper bound of critical resource | integer    |

For example:

```
./bin/pathwyse instances/spprclib/A-n54-k7-149.sppcc -o 20 -d 50 -ub 200
```

Will solve the RCSPP from node 20 to node 50, with a budget of 200 for the critical resource.

