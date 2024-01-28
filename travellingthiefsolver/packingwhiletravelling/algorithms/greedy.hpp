#pragma once

#include "travellingthiefsolver/packingwhiletravelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
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
