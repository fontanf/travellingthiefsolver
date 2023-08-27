# Travelling thief solver

A solver for various problems related to the travelling thief problem:
* The travelling thief problem
* The packing while travelling problem
* The travelling while packing problem

This code was originally developped for the [GECCO 2023 Travelling Thief Problem Competition](https://sites.google.com/view/ttp-gecco2023/home).

Authors:
* [Pierre Arvy](https://github.com/arvypierre)
* [Martin Debouté](https://github.com/mdeboute)
* [Florian Fontan](https://github.com/fontanf)

## Implemented algorithms

### Travelling thief problem

* Tree search (dynamic programming) implemented with [fontanf/treesearchsolver](https://github.com/fontanf/treesearchsolver) `-a tree_search`
  * Exact algorithm, for very small problems (~10 cities)

* Local search implemented with the sequencing module from [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) `-a local_search`
  * Returns very good solutions but only practical for small problems (< 100 cities)

* Efficient local search implemented with [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) (but not using the sequecing module) `-a efficient_genetic_local_search`
  * Returns good solutions for medium problems (< 1000 cities)

* Iterative TSP PWT TTP `-a iterative_tsp_pwt_ttp`
  * Returns good solutions for large problems (< 10000 cities)

* Iterative TSP PWT `-a iterative_tsp_pwt`
  * Quickly returns solutions for very large problems (> 10000 cities)

### Packing while travelling problem

* Dynamic programming `-a dynamic_programming`
  * Exact algorithm, for small problems

* Sequential value correction `-a sequential_value_correction`
  * Very fast, a few seconds for very large problems
  * Works well when the capacity constraint is active

* Efficient local search `-a efficient_local_search`
  * Fast, about a hundred seconds for very large problems
  * Works well when the capacity constraint is not active

### Travelling while packing problem

* Local search implemented with the sequencing module from [fontanf/localsearchsolver](https://github.com/fontanf/localsearchsolver) `-a local_search`

## Usage (command line)

Compile:
```shell
bazel build -- //...
```

Examples:

```shell
./bazel-bin/travellingthiefsolver/main -v 1 -i "data/travellingthief/gecco2023/fnl4461_n4460_bounded-strongly-corr_01.ttp" -a "iterative_tsp_pwt_ttp" -t 600
```
```
=====================================
       Travelling thief solver       
=====================================

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
       0.000           0           0           0           0           0                        
       1.104      185572      323053        1008      581050      257997           initial svc 0
       2.129      185527      326869        1021      584950      258081           initial svc 1
       3.072      185486      322610        1023      581850      259240           initial svc 2
      12.588      185815      320024        1017      580449      260425          initial ttpels
      17.923      185986      322604        1018      583649      261045          initial ttpels
      30.466      186084      323040        1010      584950      261910          initial ttpels
     187.060      186040      321531        1010      584050      262519   iteration 26 (ttpels)
     247.484      185837      323100        1028      585950      262850   iteration 53 (ttpels)
     344.162      186015      320678        1027      584548      263870  iteration 113 (ttpels)

Final statistics
----------------
Value:                        263870
Bound:                        inf
Absolute optimality gap:      inf
Relative optimality gap (%):  -nan
Time (s):                     600.42
Number of SVC calls:          698
Number of ELS calls:          50
TSP time:                     275.251
PWT time:                     39.4937
TTP time:                     285.517

Solution
--------
Number of vertices:  4461 / 4461 (100%)
Distance:            186015
Travel time:         229056
Renting cost:        320678
Number of items:     1027 / 4460 (23.0269%)
Item weight:         387148 / 387150 (99.9995%)
Item profit:         584548
Feasible:            1
Objective:           263870
```
