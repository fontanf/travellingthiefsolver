#include "travellingthiefsolver/travelling_thief/algorithms/tree_search.hpp"

using namespace travellingthiefsolver::travelling_thief;

const Output travellingthiefsolver::travelling_thief::tree_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            tree_search,
            instance.distances(),
            instance,
            parameters);
}
