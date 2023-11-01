#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

#include "travellingthiefsolver/packingwhiletravelling/algorithms/greedy.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/dynamic_programming.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/efficient_local_search.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/large_neighborhood_search.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}

