#include "travellingthiefsolver/travelling_thief/algorithms/iterative_tsp_pwt.hpp"

using namespace travellingthiefsolver::travelling_thief;

const IterativeTspPwtOutput travellingthiefsolver::travelling_thief::iterative_tsp_pwt(
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            iterative_tsp_pwt,
            instance.distances(),
            instance,
            generator,
            parameters);

}
