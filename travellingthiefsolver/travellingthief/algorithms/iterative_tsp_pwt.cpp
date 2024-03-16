#include "travellingthiefsolver/travellingthief/algorithms/iterative_tsp_pwt.hpp"

using namespace travellingthiefsolver::travellingthief;

const IterativeTspPwtOutput travellingthiefsolver::travellingthief::iterative_tsp_pwt(
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
