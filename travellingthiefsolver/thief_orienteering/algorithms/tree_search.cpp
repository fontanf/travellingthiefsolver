#include "travellingthiefsolver/thief_orienteering/algorithms/tree_search.hpp"

using namespace travellingthiefsolver::thief_orienteering;

const Output travellingthiefsolver::thief_orienteering::tree_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            tree_search,
            instance.distances(),
            instance,
            parameters);
}
