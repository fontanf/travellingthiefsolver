#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

struct LargeNeighborhoodSearchOptionalParameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;

    /** Maximum number of iterations without improvement. */
    Counter maximum_number_of_iterations_without_improvement = -1;

    double ruin_random_items_weight = 1;
    double ruin_random_cities_weight = 1;
    double ruin_consecutive_cities_weight = 1;

    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

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

    /** Number of iterations. */
    Counter number_of_iterations = 0;
};

LargeNeighborhoodSearchOutput large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        LargeNeighborhoodSearchOptionalParameters parameters = {});

}
}

