#pragma once

#include "travellingthiefsolver/packingwhiletravelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

struct DynamicProgrammingParameters: Parameters
{
};

/**
 * Dynamic programming algorithm for the packing while travelling problem.
 *
 * See:
 * - "A Fully Polynomial Time Approximation Scheme for Packing While Traveling"
 *   (Neumann et al., 2018)
 *   https://doi.org/10.1007/978-3-030-19759-9_5
 */
Output dynamic_programming(
        const Instance& instance,
        const DynamicProgrammingParameters& parameters = {});

}
}
