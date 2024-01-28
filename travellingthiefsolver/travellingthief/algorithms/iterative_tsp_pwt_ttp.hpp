#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

struct IterativeTspPwtTtpParameters: Parameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;
};

struct IterativeTspPwtTtpOutput: Output
{
    IterativeTspPwtTtpOutput(
            const Instance& instance):
        Output(instance) { }


    /** Number of SVC calls. */
    Counter number_of_svc_calls = 0;

    /** Number of efficient local search calls. */
    Counter number_of_els_calls = 0;

    /** Time spent in TSP. */
    double tsp_time = 0.0;

    /** Time spent in PWT. */
    double pwt_time = 0.0;

    /** Time spent in TTP. */
    double ttp_time = 0.0;
};

const IterativeTspPwtTtpOutput iterative_tsp_pwt_ttp(
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtTtpParameters& parameters = {});

}
}

