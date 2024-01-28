#pragma once

#include "travellingthiefsolver/packingwhiletravelling/algorithm.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

struct EfficientLocalSearchParameters: Parameters
{
    /** Solution. */
    Solution* initial_solution = nullptr;

    /**
     * Minimum relative improvement required at each iteration. If this value
     * is not reached, the algorithm stops.
     *
     * This is only used when the objective is > 0.
     */
    double minimum_improvement = 0.001;
};

struct EfficientLocalSearchOutput: Output
{
    EfficientLocalSearchOutput(
            const Instance& instance):
        Output(instance) { }


    Counter number_of_iterations = 0;
};

EfficientLocalSearchOutput efficient_local_search(
        const Instance& instance,
        const EfficientLocalSearchParameters& parameters = {});

}
}
