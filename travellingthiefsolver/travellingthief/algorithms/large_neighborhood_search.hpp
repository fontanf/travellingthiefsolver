#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

struct LargeNeighborhoodSearchOptionalParameters
{
    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

struct LargeNeighborhoodSearchOutput: Output
{
    LargeNeighborhoodSearchOutput(
            const Instance& instance,
            optimizationtools::Info& info):
        Output(instance, info) { }

    void print_statistics(
            optimizationtools::Info& info) const override;

    Counter number_of_iterations = 0;
};

/**
 * Large neighborhood search algorithm for the travelling thief problem.
 */
Output large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        LargeNeighborhoodSearchOptionalParameters parameters = {});

}
}

