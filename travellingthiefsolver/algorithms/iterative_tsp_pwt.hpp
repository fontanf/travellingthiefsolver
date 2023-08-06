#pragma once

#include "travellingthiefsolver/solution.hpp"

namespace travellingthiefsolver
{

struct IterativeTspPwtOptionalParameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

struct IterativeTspPwtOutput: Output
{
    IterativeTspPwtOutput(
            const Instance& instance,
            optimizationtools::Info& info):
        Output(instance, info) { }

    void print_statistics(
            optimizationtools::Info& info) const override;

    /** Number of SVC calls. */
    Counter number_of_svc_calls = 0;

    /** Number of efficient local search calls. */
    Counter number_of_els_calls = 0;

    /** Time spent in TSP. */
    double tsp_time = 0.0;

    /** Time spent in PWT. */
    double pwt_time = 0.0;
};

IterativeTspPwtOutput iterative_tsp_pwt(
        const Instance& instance,
        std::mt19937_64& generator,
        IterativeTspPwtOptionalParameters parameters = {});

}

