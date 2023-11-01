#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

struct EfficientLocalSearchOptionalParameters
{
    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

    /** Solution. */
    Solution* initial_solution = nullptr;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();

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
            const Instance& instance,
            optimizationtools::Info& info):
        Output(instance, info) { }

    void print_statistics(
            optimizationtools::Info& info) const override;

    Counter number_of_iterations = 0;
};

EfficientLocalSearchOutput efficient_local_search(
        const Instance& instance,
        EfficientLocalSearchOptionalParameters parameters = {});

}
}

