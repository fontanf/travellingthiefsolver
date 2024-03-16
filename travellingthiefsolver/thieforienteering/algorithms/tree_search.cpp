#include "travellingthiefsolver/thieforienteering/algorithms/tree_search.hpp"

using namespace travellingthiefsolver::thieforienteering;

const Output travellingthiefsolver::thieforienteering::tree_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            tree_search,
            instance.distances(),
            instance,
            parameters);
}
