#include "travellingthiefsolver/travellingthief/algorithms/iterative_tsp_pwt_ttp.hpp"

using namespace travellingthiefsolver::travellingthief;

const IterativeTspPwtTtpOutput travellingthiefsolver::travellingthief::iterative_tsp_pwt_ttp(
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtTtpParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            iterative_tsp_pwt_ttp,
            instance.distances(),
            instance,
            generator,
            parameters);
}
