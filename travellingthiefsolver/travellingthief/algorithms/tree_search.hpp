#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

/**
 * Tree search algorithm (dynamic programming) for the travelling thief
 * problem..
 *
 * The algorithm is close from:
 * - "Exact Approaches for the Travelling Thief Problem" (Wu et al., 2017)
 *   https://doi.org/10.1007/978-3-319-68759-9_10
 */
const Output tree_search(
        const Instance& instance,
        const Parameters& parameters = {});

}
}
