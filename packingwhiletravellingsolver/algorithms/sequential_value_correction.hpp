#pragma once

#include "packingwhiletravellingsolver/solution.hpp"

namespace packingwhiletravellingsolver
{

struct SequentialValueCorrectionOptionalParameters
{
    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

Output sequential_value_correction(
        const Instance& instance,
        SequentialValueCorrectionOptionalParameters parameters = {});

}

