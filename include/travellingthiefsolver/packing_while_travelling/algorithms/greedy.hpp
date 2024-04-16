#pragma once

#include "travellingthiefsolver/packing_while_travelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

struct GreedyParameters: Parameters
{
    /**
     * Scoring function.
     *
     * - 0: scoring function from polyakovskiy2014
     * - 1: scoring function from nieto2018
     * - 2: scoring function from polyakovskiy2014 divided by weight
     */
    int scoring_function = 0;
};

Output greedy(
        const Instance& instance,
        const GreedyParameters& parameters = {});

}
}
