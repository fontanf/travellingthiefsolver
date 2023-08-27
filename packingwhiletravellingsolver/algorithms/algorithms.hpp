#pragma once

#include "packingwhiletravellingsolver/solution.hpp"

#include "packingwhiletravellingsolver/algorithms/greedy.hpp"
#include "packingwhiletravellingsolver/algorithms/sequential_value_correction.hpp"
#include "packingwhiletravellingsolver/algorithms/dynamic_programming.hpp"
#include "packingwhiletravellingsolver/algorithms/efficient_local_search.hpp"
#include "packingwhiletravellingsolver/algorithms/large_neighborhood_search.hpp"

namespace packingwhiletravellingsolver
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}

