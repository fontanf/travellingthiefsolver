#include "travellingthiefsolver/thief_orienteering/algorithms/local_search.hpp"

using namespace travellingthiefsolver::thief_orienteering;

const Output travellingthiefsolver::thief_orienteering::local_search(
        const Instance& instance,
        const LocalSearchParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
