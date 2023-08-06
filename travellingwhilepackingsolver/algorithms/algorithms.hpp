#pragma once

#include "travellingwhilepackingsolver/solution.hpp"

#include "travellingwhilepackingsolver/algorithms/local_search.hpp"

namespace travellingwhilepackingsolver
{

Output run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}

