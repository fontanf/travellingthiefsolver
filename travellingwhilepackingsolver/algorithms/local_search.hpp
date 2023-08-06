#pragma once

#include "travellingwhilepackingsolver/solution.hpp"

namespace travellingwhilepackingsolver
{

Output local_search(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}

