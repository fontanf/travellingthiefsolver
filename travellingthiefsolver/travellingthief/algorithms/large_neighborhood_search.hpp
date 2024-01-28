#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

struct LargeNeighborhoodSearchParameters: Parameters
{
};

struct LargeNeighborhoodSearchOutput: Output
{
    LargeNeighborhoodSearchOutput(
            const Instance& instance):
        Output(instance) { }


    Counter number_of_iterations = 0;
};

/**
 * Large neighborhood search algorithm for the travelling thief problem.
 */
Output large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        const LargeNeighborhoodSearchParameters& parameters = {});

}
}
