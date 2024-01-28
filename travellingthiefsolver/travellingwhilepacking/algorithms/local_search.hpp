#pragma once

#include "travellingthiefsolver/travellingwhilepacking/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingwhilepacking
{

Output local_search(
        const Instance& instance,
        const Parameters& parameters = {});

}
}
