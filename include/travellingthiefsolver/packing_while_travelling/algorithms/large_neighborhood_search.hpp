#pragma once

#include "travellingthiefsolver/packing_while_travelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

struct LargeNeighborhoodSearchParameters: Parameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;

    /** Maximum number of iterations without improvement. */
    Counter maximum_number_of_iterations_without_improvement = -1;

    double ruin_random_items_weight = 1;
    double ruin_random_cities_weight = 1;
    double ruin_consecutive_cities_weight = 1;
};

struct LargeNeighborhoodSearchOutput: Output
{
    LargeNeighborhoodSearchOutput(
            const Instance& instance):
        Output(instance) { }


    /** Number of iterations. */
    Counter number_of_iterations = 0;
};

LargeNeighborhoodSearchOutput large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        const LargeNeighborhoodSearchParameters& parameters = {});

}
}
