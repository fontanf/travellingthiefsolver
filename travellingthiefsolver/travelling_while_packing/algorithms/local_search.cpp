#include "travellingthiefsolver/travelling_while_packing/algorithms/local_search.hpp"

using namespace travellingthiefsolver::travelling_while_packing;

const Output travellingthiefsolver::travelling_while_packing::local_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
