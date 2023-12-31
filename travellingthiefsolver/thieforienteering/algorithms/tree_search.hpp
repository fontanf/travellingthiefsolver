#pragma once

#include "travellingthiefsolver/thieforienteering/solution.hpp"

namespace travellingthiefsolver
{
namespace thieforienteering
{

/**
 * Tree search algorithm (dynamic programming) for the thief orienteering
 * problem..
 *
 * The algorithm is close from:
 * - "Exact Approaches for the Travelling Thief Problem" (Wu et al., 2017)
 *   https://doi.org/10.1007/978-3-319-68759-9_10
 */
const Output tree_search(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}
}

