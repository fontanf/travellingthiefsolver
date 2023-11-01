#pragma once

#include "travellingthiefsolver/thieforienteering/solution.hpp"

#include "travellingthiefsolver/thieforienteering/algorithms/tree_search.hpp"
#include "travellingthiefsolver/thieforienteering/algorithms/local_search.hpp"

namespace travellingthiefsolver
{
namespace thieforienteering
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}
