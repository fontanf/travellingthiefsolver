#pragma once

#include "travellingthiefsolver/travellingwhilepacking/solution.hpp"

#include "travellingthiefsolver/travellingwhilepacking/algorithms/local_search.hpp"

namespace travellingthiefsolver
{
namespace travellingwhilepacking
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}

