# Travelling thief solver

A solver for various problems related to the travelling thief problem:
* The travelling thief problem
* The packing while travelling problem
* The travelling while packing problem
* The thief orienteering problem

This code was originally developped for the [GECCO 2023 Travelling Thief Problem Competition](https://sites.google.com/view/ttp-gecco2023/home).

Authors:
* [Pierre Arvy](https://github.com/arvypierre)
* [Martin Debouté](https://github.com/mdeboute)
* [Florian Fontan](https://github.com/fontanf)

## Implemented algorithms

### Travelling thief problem

* Tree search (dynamic programming) implemented with [fontanf/treesearchsolver](https://github.com/fontanf/treesearchsolver) `-a tree-search`
  * Exact algorithm, for very small problems (~10 cities)

* Local search implemented with the sequencing module from [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) `-a local-search`
  * Returns very good solutions but only practical for small problems (< 100 cities)

* Efficient local search implemented with [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) (but not using the sequecing module) `-a efficient-genetic-local-search`
  * Returns good solutions for medium problems (< 1000 cities)

* Iterative TSP PWT TTP `-a iterative-tsp-pwt-ttp`
  * Returns good solutions for large problems (< 10000 cities)

* Iterative TSP PWT `-a iterative-tsp-pwt`
  * Quickly returns solutions for very large problems (> 10000 cities)

### Packing while travelling problem

* Dynamic programming `-a dynamic-programming`
  * Exact algorithm, for small problems

* Sequential value correction `-a sequential-value-correction`
  * Very fast, a few seconds for very large problems
  * Works well when the capacity constraint is active

* Efficient local search `-a efficient-local-search`
  * Fast, about a hundred seconds for very large problems
  * Works well when the capacity constraint is not active

### Travelling while packing problem

* Local search implemented with the sequencing module from [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) `-a local-search`

### Thief orienteering problem

* Tree search (dynamic programming) implemented with [fontanf/treesearchsolver](https://github.com/fontanf/treesearchsolver) `-a tree-search`
  * Exact algorithm, for very small problems (~10 cities)

* Local search implemented with the sequencing module from [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) `-a local-search`
  * Returns very good solutions but only practical for small problems (< 100 cities)

## Usage (command line)

Compile:
```shell
bazel build -- //...
```

Examples:

```shell
./bazel-bin/travellingthiefsolver/travellingthief/main -v 1 -i "data/travellingthief/gecco2023/fnl4461_n4460_bounded-strongly-corr_01.ttp" -a "iterative-tsp-pwt-ttp" -t 600
```
```
=====================================
        TravellingThiefSolver        
=====================================

Problem
-------
Travelling thief problem

Instance
--------
Number of cities:  4461
Number of items:   4460
Capacity:          387150
Minimum speed:     0.1
Maximum speed:     1
Renting ratio:     1.4
Profit sum:        5.11346e+06
Weight sum:        4258655
Weight ratio:      11

Algorithm
---------
Iterative TSP PWT TTP

       T (s)    Distance        Cost     # items      Profit   Objective                 Comment
       -----    --------        ----     -------      ------   ---------                 -------
       0.003           0           0           0           0           0                        
       1.022      185572      323053        1008      581050      257997           initial svc 0
       2.031      185527      326869        1021      584950      258081           initial svc 1
       2.966      185486      322610        1023      581850      259240           initial svc 2
      18.614      185815      320024        1017      580449      260425          initial ttpels
      28.648      185986      322604        1018      583649      261045          initial ttpels
      52.541      186206      323108        1011      584650      261542          initial ttpels
     260.383      185837      322384        1022      584350      261966    iteration 8 (ttpels)
     326.622      186310      319930        1020      583243      263313   iteration 16 (ttpels)

Final statistics
----------------
Value:                        263313
Bound:                        inf
Absolute optimality gap:      inf
Relative optimality gap (%):  -nan
Time (s):                     600.479
Number of SVC calls:          226
Number of ELS calls:          40
TSP time:                     90.7668
PWT time:                     14.5937
TTP time:                     495.084

Solution
--------
Number of vertices:  4461 / 4461 (100%)
Distance:            186310
Travel time:         228521
Renting cost:        319930
Number of items:     1020 / 4460 (22.87%)
Item weight:         387143 / 387150 (99.9982%)
Item profit:         583243
Feasible:            1
Objective:           263313
```
