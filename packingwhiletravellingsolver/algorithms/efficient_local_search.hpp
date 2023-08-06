#pragma once

#include "packingwhiletravellingsolver/solution.hpp"

namespace packingwhiletravellingsolver
{

struct EfficientLocalSearchOptionalParameters
{
    /** Reduction parameters. */
    ReductionParameters reduction_parameters;

    /** Solution. */
    Solution* initial_solution = nullptr;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
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

