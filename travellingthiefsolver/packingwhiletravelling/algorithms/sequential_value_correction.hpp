#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
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
}

