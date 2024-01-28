#pragma once

#include "travellingthiefsolver/packingwhiletravelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

struct SequentialValueCorrectionParameters: Parameters
{
};

Output sequential_value_correction(
        const Instance& instance,
        const SequentialValueCorrectionParameters& parameters = {});

}
}

