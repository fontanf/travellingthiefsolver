#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

#include "travellingthiefsolver/travellingthief/algorithms/tree_search.hpp"
#include "travellingthiefsolver/travellingthief/algorithms/local_search.hpp"
#include "travellingthiefsolver/travellingthief/algorithms/efficient_local_search.hpp"
#include "travellingthiefsolver/travellingthief/algorithms/iterative_tsp_pwt.hpp"
#include "travellingthiefsolver/travellingthief/algorithms/iterative_tsp_pwt_ttp.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}

