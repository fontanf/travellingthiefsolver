#pragma once

#include "packingwhiletravellingsolver/solution.hpp"

namespace packingwhiletravellingsolver
{

struct DynamicProgrammingOptionalParameters
{
    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

Output dynamic_programming(
        const Instance& instance,
        DynamicProgrammingOptionalParameters parameters = {});

}
