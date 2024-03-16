#include "travellingthiefsolver/travellingthief/algorithms/tree_search.hpp"

using namespace travellingthiefsolver::travellingthief;

const Output travellingthiefsolver::travellingthief::tree_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            tree_search,
            instance.distances(),
            instance,
            parameters);
}
