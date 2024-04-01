#pragma once

#include "travellingthiefsolver/packing_while_travelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

struct SequentialValueCorrectionParameters: Parameters
{
};

Output sequential_value_correction(
        const Instance& instance,
        const SequentialValueCorrectionParameters& parameters = {});

}
}

