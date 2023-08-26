#pragma once

#include "travellingthiefsolver/solution.hpp"

#include "travellingthiefsolver/algorithms/tree_search.hpp"
#include "travellingthiefsolver/algorithms/local_search.hpp"
#include "travellingthiefsolver/algorithms/efficient_local_search.hpp"
#include "travellingthiefsolver/algorithms/iterative_tsp_pwt.hpp"
#include "travellingthiefsolver/algorithms/iterative_tsp_pwt_ttp.hpp"

namespace travellingthiefsolver
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}

