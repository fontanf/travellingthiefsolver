#include "travellingthiefsolver/travelling_thief/algorithms/local_search.hpp"

using namespace travellingthiefsolver::travelling_thief;

const Output travellingthiefsolver::travelling_thief::local_search(
        const Instance& instance,
        const LocalSearchParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
