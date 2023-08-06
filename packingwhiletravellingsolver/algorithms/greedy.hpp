#pragma once

#include "packingwhiletravellingsolver/solution.hpp"

namespace packingwhiletravellingsolver
{

struct GreedyOptionalParameters
{
    /**
     * Scoring function.
     *
     * - 0: scoring function from polyakovskiy2014
     * - 1: scoring function from nieto2018
     * - 2: scoring function from polyakovskiy2014 divided by weight
     */
    int scoring_function = 0;

    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

Output greedy(const Instance& instance, GreedyOptionalParameters parameters = {});

}