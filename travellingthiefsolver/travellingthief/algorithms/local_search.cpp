#include "travellingthiefsolver/travellingthief/algorithms/local_search.hpp"

using namespace travellingthiefsolver::travellingthief;

const Output travellingthiefsolver::travellingthief::local_search(
        const Instance& instance,
        const LocalSearchParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
