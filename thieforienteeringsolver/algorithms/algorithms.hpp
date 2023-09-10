#pragma once

#include "thieforienteeringsolver/solution.hpp"

#include "thieforienteeringsolver/algorithms/tree_search.hpp"
#include "thieforienteeringsolver/algorithms/local_search.hpp"

namespace thieforienteeringsolver
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
