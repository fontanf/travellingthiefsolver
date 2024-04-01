#include "travellingthiefsolver/travelling_thief/algorithms/iterative_tsp_pwt_ttp.hpp"

using namespace travellingthiefsolver::travelling_thief;

const IterativeTspPwtTtpOutput travellingthiefsolver::travelling_thief::iterative_tsp_pwt_ttp(
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
