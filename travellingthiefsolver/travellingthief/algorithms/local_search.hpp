#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

/**
 * Local search algorithm for the travelling thief problem.
 *
 * The implementation is inspired by:
 * - "Investigation of the Traveling Thief Problem" (Wuijts, 2018)
 *   https://studenttheses.uu.nl/handle/20.500.12932/29179
 * - "Investigation of the traveling thief problem" (Wuijts et Thierens, 2019)
 *   https://doi.org/10.1145/3321707.3321766
 */
const Output local_search(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}
}

